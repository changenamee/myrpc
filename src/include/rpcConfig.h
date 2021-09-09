#pragma once

#include<unordered_map>
#include<string>

class rpcConfig{
public:
    void LoadConfigFile(const char* file);
    std::string Load(const std::string& key);
private:
    std::unordered_map<std::string,std::string> configMap;
    void Trim(std::string& str);
};