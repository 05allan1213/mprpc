#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char **argv)
{
    // 想使用mprpc框架来调用rpc方法，需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 调用UserService rpc方法
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应参数
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);
    // 打印响应结果
    if (response.result().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.sucess() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    return 0;
}