#pragma once
#include<string>
#include<google/protobuf/service.h>

class rpcController:public google::protobuf::RpcController
{
public:
    rpcController();
    void ReSet();
    bool Failed() const;
    std::string ErrText() const;
    void SetFailed(const std::string& errMesg);

    //目前未实现具体的功能
    void StartCancel();
    bool IsCanceled()const;
    void NotifyOnCancel(google::protobuf::Closure *callback);
private:
    bool failed;
    std::string m_errtext;
};