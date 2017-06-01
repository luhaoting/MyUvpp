#include "TcpConn.h"

TcpConn::TcpConn(uv::Loop& loop)
    : m_rLoop(loop),
    m_handle(loop),
    mf_close_cb([] {})
{
}

TcpConn::~TcpConn()
{
}

void TcpConn::SetReadCb()
{
    m_handle.read_start(std::bind(&TcpConn::OnRecv, this, std::placeholders::_1, std::placeholders::_2));
}

void TcpConn::OnRecv(const char *buff, ssize_t len)
{
    if (len > 0)
    {
        //这里应有解包
        std::string str;
        str.append(buff, len);

        //free(buff);
        std::cout << str << std::endl;
        return;
    }
    else
    {
        m_handle.close(mf_close_cb);
    }
}

void TcpConn::SendTo(std::string& strMsg)
{
    m_handle.write(strMsg.c_str(), strMsg.length(), std::bind(&TcpConn::OnWriteFinished, this, std::placeholders::_1));
}

void TcpConn::ConnectTo(std::string & strIp, const int nPort)
{
    m_handle.connect(strIp, nPort, nullptr);
}

void TcpConn::OnWriteFinished(uv::Error error)
{
    if (!error)
    {
        std::cout << error.str() << std::endl;
    }
}
