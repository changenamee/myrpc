#pragma once

#include"rpcConfig.h"

class rpcApplication{
public:
    static void Init(int argc,char* argv[]);
    static rpcApplication& GetInstance();
    static rpcConfig& GetConfig();
    
private:
    static rpcConfig config;
    rpcApplication(){}
    rpcApplication(const rpcApplication&) = delete;
    rpcApplication(rpcApplication&&) = delete;
};
