#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

int main(int argc, char **argv)
{
    // 想使用mprpc框架来调用rpc方法，需要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 调用UserService rpc方法
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    // rpc login方法的请求参数
    fixbug::LoginRequest request;
    request.set_name(std::string("zhang san"));
    request.set_pwd(std::string("123456"));
    // rpc login方法的响应参数
    fixbug::LoginResponse response;
    MprpcController controller1;
    stub.Login(&controller1, &request, &response, nullptr);
    // 一次rpc调用完成，返回结果
    if (controller1.Failed())
    {
        std::cout << "rpc login response error: " << controller1.ErrorText() << std::endl;
    }
    else
    {
        // 打印响应结果
        if (response.result().errcode() == 0)
        {
            std::cout << "rpc login response success: " << response.sucess() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }

    // rpc register方法的请求参数
    fixbug::RegisterRequest register_request;
    register_request.set_id(2000);
    register_request.set_name(std::string("li si"));
    register_request.set_pwd(std::string("123456"));
    // rpc register方法的响应参数
    fixbug::RegisterResponse register_response;
    MprpcController controller2;
    stub.Register(&controller2, &register_request, &register_response, nullptr);
    // 一次rpc调用完成，返回结果
    if (controller2.Failed())
    {
        std::cout << "rpc register response error: " << controller2.ErrorText() << std::endl;
    }
    else
    {
        // 打印响应结果
        if (register_response.result().errcode() == 0)
        {
            std::cout << "rpc register response success: " << register_response.sucess() << std::endl;
        }
        else
        {
            std::cout << "rpc register response error: " << register_response.result().errmsg() << std::endl;
        }
    }

    return 0;
}