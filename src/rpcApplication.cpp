#include"include/rpcApplication.h"
#include<iostream>
#include<unistd.h>

static rpcConfig config;

void showErr(){
    std::cout<<"command -i <>"<<std::endl;
}

void rpcApplication::Init(int argc,char* argv[]){
    if(argc<2){
        showErr();
        return;
    }

    int c=0;
    std::string configInfo;

    while((c = getopt(argc,argv,"i:"))!=-1){
        switch (c)
        {
        case 'i':
            configInfo = optarg;
            break;
        case '?':
            showErr();
            exit(EXIT_FAILURE);
        case ':':
            std::cout<<"need argument"<<std::endl;
            showErr();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    //load configfile
    config.LoadConfigFile(configInfo.c_str());
}

//获取实例，单例模式
rpcApplication& rpcApplication::GetInstance(){
    static rpcApplication instance;
    return instance;
}

rpcConfig& rpcApplication::GetConfig(){
    return config;
}


