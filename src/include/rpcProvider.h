#pragma once
#include<google/protobuf/service.h>
#include<google/protobuf/descriptor.h>  
#include<muduo/net/TcpConnection.h>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<unordered_map>
#include<string>
#include<functional>


class rpcProvider{
public:
    void PublishService(google::protobuf::Service* service);

    void Run();
private:
    muduo::net::EventLoop m_eventLoop;

    struct serviceInfo{
        google::protobuf::Service* m_service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;
    };

    std::unordered_map<std::string,serviceInfo> m_serviceMap;


    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);
};