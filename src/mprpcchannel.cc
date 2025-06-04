#include "mprpcchannel.h"
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "rpcheader.pb.h"
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

/*
header_size + service_name + method_name + args_size + args
*/

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *service = method->service();
    std::string service_name = std::string(service->name());
    std::string method_name = std::string(method->name());

    // 获取参数的序列化字符串长度
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("request serialize to string error!");
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    // 将header序列化
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpc_header.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("rpc_header serialize to string error!");
        return;
    }

    // 将header_size、service_name、method_name、args_size、args序列化
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4)); // header_size
    send_rpc_str += rpc_header_str;                               // rpc_header
    send_rpc_str += args_str;                                     // args

    // 打印调试信息
    std::cout << "===========================================" << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===========================================" << std::endl;

    // 使用tcp客户端发送rpc请求
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        char err_info[512] = {0};
        sprintf(err_info, "create socket error! errno: %d", errno);
        controller->SetFailed(err_info);
        return;
    }

    // 读取配置文件rpcserver的ip和port
    std::string ip = MprpcApplication::GetInstance()->GetConfig()->Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance()->GetConfig()->Load("rpcserverport").c_str());

    // 创建socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务器
    if (connect(clientfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        close(clientfd);
        char err_info[512] = {0};
        sprintf(err_info, "connect error! errno: %d", errno);
        controller->SetFailed(err_info);
        return;
    }

    // 发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        char err_info[512] = {0};
        sprintf(err_info, "send error! errno: %d", errno);
        controller->SetFailed(err_info);
        close(clientfd);
        return;
    }

    // 接收rpc响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, sizeof(recv_buf), 0)) == -1)
    {
        close(clientfd);
        char err_info[512] = {0};
        sprintf(err_info, "recv error! errno: %d", errno);
        controller->SetFailed(err_info);
        return;
    }

    // 反序列化rpc调用的响应
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        close(clientfd);
        char err_info[512] = {0};
        sprintf(err_info, "parse response error! response_str: %s", std::string(recv_buf, recv_size).c_str());
        controller->SetFailed(err_info);
        return;
    }

    // 关闭socket
    close(clientfd);
}