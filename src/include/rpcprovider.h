#pragma once

#include <google/protobuf/service.h>
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

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
    // 上报连接信息的回调函数
    void OnConnection(const muduo::net::TcpConnectionPtr &conn);
    // 上报读写事件信息的回调函数
    void OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp receiveTime);
};