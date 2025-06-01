#include "rpcprovider.h"
#include "mprpcapplication.h"

void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;
    serviceInfo.m_service = service;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *serviceDescriptor = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = std::string(serviceDescriptor->name());
    // 获取服务对象service的方法的数量
    int methodCount = serviceDescriptor->method_count();
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "methodCount: " << methodCount << std::endl;

    for (int i = 0; i < methodCount; ++i)
    {
        // 获取方法的描述信息
        const google::protobuf::MethodDescriptor *methodDescriptor = serviceDescriptor->method(i);
        std::string method_name = std::string(methodDescriptor->name());
        serviceInfo.m_methodMap.insert({method_name, methodDescriptor});
    }
    m_serviceMap.insert({service_name, serviceInfo});
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance()->GetConfig()->Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance()->GetConfig()->Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start at ip: " << ip << " port: " << port << std::endl;

    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client的连接断开了
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp receiveTime)
{
}