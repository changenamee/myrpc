#include"rpcHeader.pb.h"
#include"include/rpcProvider.h"
#include"include/zookeeperUtil.h"
#include"include/rpcApplication.h"
//发布RPC方法
void rpcProvider::PublishService(google::protobuf::Service* service){
    serviceInfo service_info;

    //服务对象描述信息
    const google::protobuf::ServiceDescriptor* pserviceDes = service->GetDescriptor();

    //服务名称
    std::string service_name = pserviceDes->name();

    //服务方法数量
    uint32_t methodCnt = pserviceDes->method_count();


    for(int i=0;i<methodCnt;++i){
        const google::protobuf::MethodDescriptor* pmethodDes = pserviceDes->method(i);
        //方法名称
        std::string method_name = pmethodDes->name();

        service_info.m_methodMap.insert({method_name,pmethodDes});
    }

    service_info.m_service = service;
    m_serviceMap.insert({service_name,service_info});
}

void rpcProvider::Run(){
    std::string ip = rpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint32_t port = atoi(rpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip,port);

    muduo::net::TcpServer server(&m_eventLoop,address,"rpcProvider");

    //绑定回调函数
    server.setConnectionCallback(std::bind(&rpcProvider::OnConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&rpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    //set thread numbers
    server.setThreadNum(4);

    //将RPC服务注册到zk上，rpc client从zk发现服务
    ZKClient zkclient;
    //连接zkserver
    zkclient.Start();
    
    for(auto& service:m_serviceMap){
        //service path
        std::string service_path = "/"+service.first;
        //create tmp node
        zkclient.Create(service_path.c_str(),nullptr,0,0);

        for(auto& method:service.second.m_methodMap){
            //service_path/method_path
            std::string method_path = service_path+"/"+method.first;
            char method_path_data[512];
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            //ZOO_EPHEMERAL临时节点
            zkclient.Create(method_path.c_str(),method_path_data,sizeof(method_path_data),ZOO_EPHEMERAL);
        }
    }

    std::cout<<"rpcProvider start service at "<<ip<<":"<<port<<std::endl;

    server.start();
    m_eventLoop.loop();
}

void rpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
    //和rpcclient连接断开
    if(!conn->connected()){
        conn->shutdown();
    }
}

void rpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp to){
    //获取数据
    std::string recvBuf = buffer->retrieveAllAsString();

    //读取数据头大小
    uint32_t header_size;
    recvBuf.copy((char*)&header_size,4,0);

    myrpc::rpcHeader rh;

    //获取头数据

    std::string rpc_header_str = recvBuf.substr(4,header_size);
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rh.ParseFromString(rpc_header_str)){
        service_name = rh.service_name();
        method_name = rh.method_name();
        args_size = rh.args_size();
    }else{
        //反序列化失败
        std::cout<<"fail to parse from header infomation"<<std::endl;
        return;
    }

    std::string args_str = recvBuf.substr(4+header_size,args_size);

    //get service
    auto it = m_serviceMap.find(service_name);

    if(it == m_serviceMap.end()){
        std::cout<<service_name<<" is not exists"<<std::endl;
        return;
    }
    //get method
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end()){
        std::cout<<method_name<<" is not exists"<<std::endl;
        return;
    }


    google::protobuf::Service* service = it->second.m_service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    //rrpc request and response
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();

    if(!request->ParseFromString(args_str)){
        std::cout<<"parse args fail"<<std::endl;
        return;
    }

    google::protobuf::Message* response = service->GetRequestPrototype(method).New();

    google::protobuf::Closure* done = google::protobuf::NewCallback
    <rpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
    (this,&rpcProvider::SendRpcResponse,conn,response);
    //rpc call
    service->CallMethod(method,nullptr,request,response,done);

}

void rpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response){
    std::string response_str;

    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
    }else{
        std::cout<<"fail tp serialize to string"<<std::endl;

    }

    conn->shutdown();
}







