#include"include/zookeeperUtil.h"
#include"include/rpcApplication.h"
#include<semaphore.h>
#include<iostream>

void watcher(zhandle_t* zh,int type,int state,const char* path,void* watcherCtx){
    if(type == ZOO_SESSION_EVENT){
        if(state == ZOO_CONNECTED_STATE){
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}



ZKClient::ZKClient():m_zhandle(nullptr){}

ZKClient::~ZKClient(){
    if(m_zhandle!=nullptr){
        zookeeper_close(m_zhandle);
    }
}

void ZKClient::Start(){
    std::string ip = rpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = rpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = ip+":"+port;


    //初始化
    m_zhandle = zookeeper_init(connstr.c_str(),watcher,30000,nullptr,nullptr,0);
    if(nullptr == m_zhandle){
        std::cout<<"fail to init zookeeper"<<std::endl;
    }

    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle,&sem);

    sem_wait(&sem);
    std::cout<<"zookeeper initialize success"<<std::endl;
}

void ZKClient::Create(const char* path,const char* data,int datalen,int state){
    char buffer[512];
    int buffer_len = sizeof(buffer);
    int flag;

    flag = zoo_exists(m_zhandle,path,0,nullptr);
    //不存在
    if(flag!=ZOK){
        flag = zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,buffer,buffer_len);
        if(flag!=ZOK){
            std::cout<<"fail to create znode"<<std::endl;
            return;
        }else{
            std::cout<<"create success"<<std::endl;
        }
    }
}

std::string ZKClient::GetData(const char* path){
    char buffer[512];
    int buffer_len = sizeof(buffer);

    int flag = zoo_get(m_zhandle,path,0,buffer,&buffer_len,nullptr);
    
    if(flag!=ZOK){
        std::cout<<path<<" is not exists"<<std::endl;
        return "";
    }
    return buffer;
}

