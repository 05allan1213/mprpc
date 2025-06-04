#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和Register
*/

class UserService : public fixbug::UserServiceRpc // 使用在rpc服务发布端（rpc服务提供者）
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id:" << id << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }

    // 重写基类UserServiceRpc的虚函数 下面这些方法都是由框架直接调用的
    void Login(::google::protobuf::RpcController *controller, const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response, ::google::protobuf::Closure *done)
    {
        // 从 request 对象中获取参数
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 执行本地业务逻辑
        bool login_result = Login(name, pwd);

        // 填充 response 对象
        response->set_sucess(login_result); // 业务是否成功

        fixbug::ResultCode *code = response->mutable_result(); // 获取响应对象的result成员

        if (login_result)
        {
            code->set_errcode(0); // 0 表示成功
            code->set_errmsg(""); // 成功则错误消息为空
        }
        else
        {
            code->set_errcode(1);              // 非0表示错误
            code->set_errmsg("Login failed!"); // 具体的错误信息
        }

        // 执行回调操作
        done->Run();
    }

    // 重写基类UserServiceRpc的虚函数 下面这些方法都是由框架直接调用的
    void Register(::google::protobuf::RpcController *controller, const ::fixbug::RegisterRequest *request,
                  ::fixbug::RegisterResponse *response, ::google::protobuf::Closure *done)
    {
        // 从 request 对象中获取参数
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 执行本地业务逻辑
        bool register_result = Register(id, name, pwd);

        // 填充 response 对象
        response->set_sucess(register_result); // 业务是否成功

        fixbug::ResultCode *code = response->mutable_result(); // 获取响应对象的result成员

        if (register_result)
        {
            code->set_errcode(0); // 0 表示成功
            code->set_errmsg(""); // 成功则错误消息为空
        }
        else
        {
            code->set_errcode(1);                 // 非0表示错误
            code->set_errmsg("Register failed!"); // 具体的错误信息
        }

        // 执行回调操作
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // 创建一个rpc服务对象并让它在发布端监听端口
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点  Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}