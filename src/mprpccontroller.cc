#include "mprpccontroller.h"

MprpcController::MprpcController()
{
    m_failed = false;
    m_errText = "";
    m_callback = nullptr;
}

void MprpcController::Reset()
{
    m_failed = false;
    m_errText = "";
    m_callback = nullptr; // 重置时也应清空回调
}

bool MprpcController::Failed() const
{
    return m_failed;
}

std::string MprpcController::ErrorText() const
{
    return m_errText;
}

void MprpcController::StartCancel()
{
    // 目前这个函数可以暂时不实现具体逻辑
    // 如果需要支持取消操作，这里将包含取消逻辑
}

void MprpcController::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}

bool MprpcController::IsCanceled() const
{
    // 目前这个函数可以暂时不实现具体逻辑，或者根据实际需求返回状态
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback)
{
    m_callback = callback;
    // 如果需要支持取消操作，当取消发生时，会调用 m_callback->Run()
}