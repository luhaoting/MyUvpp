#include "Client.h"

Client::Client(uv::Loop& loop) 
    :TcpConn(loop)
{
}


void Client::Start(string strIp, int nPort)
{
    if (!m_handle.connect(strIp, nPort, std::bind(&Client::OnConnected, this, placeholders::_1)))
    {
        return;
    }
}

void Client::OnConnected(Error error)
{
    m_handle.read_start(std::bind(&Client::OnRecv, this, placeholders::_1, placeholders::_2));
}


void Client::Send(string data)
{
    auto write_cb = [](uv::Error error)
    {
        //check is write finished!
    };

    m_handle.write(data, write_cb);
}

void Client::OnRecv(const char* buff, size_t len)
{
    string msg;
    msg.append(buff, len);
    std::cout << "[Recv] :" << msg << std::endl;
}

