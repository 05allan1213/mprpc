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
    // stub.Login();
    return 0;
}