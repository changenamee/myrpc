#include"include/rpcConfig.h"
#include<iostream>

//Load configFile
void rpcConfig::LoadConfigFile(const char* file){
    FILE* fd = fopen(file,"r");

    //fail to open file
    if(nullptr == fd){
        std::cout<<"fail to open file "<<file<<std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(fd)){
        char buffer[512];
        fgets(buffer,512,fd);

        std::string fileBuf(buffer);
        //注释
        if(fileBuf[0]=='#' || fileBuf.empty()){
            continue;
        }

        std::string key;
        std::string val;

        int idx = fileBuf.find('=');
        if(idx==-1){
            continue;
        }

        key = fileBuf.substr(0,idx);
        Trim(key);

        int idx1 = fileBuf.find('\n',idx);
        val = fileBuf.substr(idx+1,idx1-idx-1);
        Trim(val);

        configMap.insert({key,val});
    }
}

//Load key
std::string rpcConfig::Load(const std::string& key){
    auto it = configMap.find(key);

    if(it == configMap.end()){
        std::cout<<"key:"<<key<<" is not exists"<<std::endl;
        return "";
    }

    return it->second;
}
//delete space of begin and end
void rpcConfig::Trim(const std::string& str){
    int idx = str.find_first_not_of(' ');
    if(idx != -1){
        str = str.substr(idx);
    }

    int idx1 = str.find_last_not_of(' ');
    if(idx1 != -1){
        str = str.substr(0,idx1+1);
    }
}


