#pragma once

#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>

// 框架提供的专门发布rpc服务的类
class RpcProvider
{
public:
    // 这个函数是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    // 组合的muduo库的线程池
    muduo::net::EventLoop m_eventLoop;

    // service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;                                                    // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; // 保存服务方法的映射关系
    };

    // 保存注册成功的服务对象和其方法的映射关系
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    // 上报连接信息的回调函数
    void OnConnection(const muduo::net::TcpConnectionPtr &conn);
    // 上报读写事件信息的回调函数
    void OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp receiveTime);

    // Closure的回调操作，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response);
};