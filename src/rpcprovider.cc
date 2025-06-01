#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

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
    // 遍历服务对象的所有方法
    for (int i = 0; i < methodCount; ++i)
    {
        // 获取方法的描述信息
        const google::protobuf::MethodDescriptor *methodDescriptor = serviceDescriptor->method(i);
        // 获取方法的名称
        std::string method_name = std::string(methodDescriptor->name());
        // 保存方法的名称和方法的描述信息
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
        conn->shutdown();
    }
}

/*
    在框架内部，RpcProvider和RpcConsumer协商好之间通信用的protobuf数据类型
    service_name method_name args
    header_size(4个字节) + header_str + args_str
*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp receiveTime)
{
    // 网络上接收的远程rpc调用请求的字节流
    std::string recv_buf = buf->retrieveAllAsString();
    // 从recv_buf中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);
    // 根据header_size读取数据头的原始字符串数据
    std::string header_str = recv_buf.substr(4, header_size);
    // 将header_str字符串反序列化为mprpc::RpcHeader对象
    mprpc::RpcHeader rpcHeader;
    bool success = rpcHeader.ParseFromString(header_str);
    if (!success)
    {
        // 数据头反序列化失败
        std::cout << "header_str: " << header_str << " parse error!" << std::endl;
        return;
    }
    // 获取service_name和method_name
    std::string service_name = rpcHeader.service_name();
    std::string method_name = rpcHeader.method_name();
    // 获取参数的序列化字符串长度
    uint32_t args_size = rpcHeader.args_size();
    // 获取参数的序列化字符串
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    // 打印调试信息
    std::cout << "===========================================" << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===========================================" << std::endl;

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        std::cout << service_name << "is not exist!" << std::endl;
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << "is not exist!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;      // 获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second; // 获取method对象

    // 生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method方法的调用，绑定一个Closure的回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse, conn, response);

    // 在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送给rpc的调用方
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}