#pragma once
#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController
{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void StartCancel();
    void SetFailed(const std::string &reason);
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);

private:
    bool m_failed;                         // RPC方法执行过程中的状态
    std::string m_errText;                 // RPC方法执行过程中的错误信息
    google::protobuf::Closure *m_callback; // RPC方法执行过程中的回调函数
};