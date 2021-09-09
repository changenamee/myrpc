#include"include/rpcChannel.h"
#include"include/zookeeperUtil.h"
#include"rpcHeader.pb.h"

#include<string>
#include<unistd.h>

#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>


//rpc方法的序列化与数据传送
void rpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,google::protobuf::Message* response,
                            google::protobuf::Closure* done){
    
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    //获取参数序列化长度和参数数据
    std::string args_str;
    uint32_t args_size = 0;


    //序列化请求参数
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }else{
        //std::cout<<"fail to serialize request"<<std::endl;
        controller->SetFailed("fail to serialize request");
        return;
    }

    //设置header参数
    myrpc::rpcHeader rh;
    rh.set_service_name(service_name);
    rh.set_method_name(method_name);
    rh.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string header_str;
    if(rh.SerializeToString(&header_str)){
        header_size = header_str.size();
    }else{
        //fail to serialize
        controller->SetFailed("fail to serialize");
        return;
    }

    //msg to send
    std::string send_str;
    send_str.insert(0,std::string((char*)&header_size,4));
    send_str += header_str;
    send_str += args_str;

    //network 
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientfd){
        close(clientfd);
        controller->SetFailed("fail to create socket");
        return;
    }


    //向zookeeper查询service节点
    ZKClient zkclient;
    zkclient.Start();
    std::string method_path = "/"+service_name+"/"+method_name;
    //获取服务地址信息
    std::string addr = zkclient.GetData(method_path.c_str());

    if(addr == ""){
        close(clientfd);
        controller->SetFailed("method is not exists");
        return;
    }

    int index = addr.find(":");
    if(index == -1){
        close(clientfd);
        controller->SetFailed("invalid addr");
        return;
    }
    //get service ip and port
    std::string ip = addr.substr(0,index);
    uint32_t port = atoi(addr.substr(index+1).c_str());

    //set address information
    struct sockaddr_in addrin;
    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = inet_addr(ip.c_str());
    addrin.sin_port = htons(port);

    //connect
    if(-1 == connect(clientfd,(struct sockaddr*)&addrin,sizeof(addrin))){
        close(clientfd);
        controller->SetFailed("fail to connect to service node");
        return;
    }

    if(-1 == send(clientfd,send_str.c_str(),sizeof(send_str),0)){
        close(clientfd);
        controller->SetFailed("fail to send request args");
        return;
    }

    //reveive from service node
    char recv_buf[1024];
    uint32_t recv_size = 0;
    if(-1 == (recv_size = recv(clientfd,recv_buf,1024,0))){
        close(clientfd);
        controller->SetFailed("fail to receive from service node");
        return;
    }

    //std::string response_str(recv_buf,0,recv_size);

    //反序列化response
    //ParseFromString会遇到'\0'导致数据读取不完整，所以用array
    if(!response->ParsePartialFromArray(&recv_buf,recv_size)){
        controller->SetFailed("fail to parse from response_str");
        close(clientfd);
        return;
    }
    close(clientfd);
}   











