#include "TcpConn.h"

TcpConn::TcpConn(uv::Loop& loop) 
    : m_rLoop(loop),
    m_handle(loop)
{
    m_handle.read_start(std::bind(&TcpConn::OnRecv, this, std::placeholders::_1, std::placeholders::_2));
}

TcpConn::~TcpConn()
{
}

void TcpConn::OnRecv(const char *buff, ssize_t len)
{
    while (true)
    {
        //����Ӧ�н��
        std::string str;
        str.append(buff, len);

        //free(buff);
        std::cout << str << std::endl;
        return;
    }
}

void TcpConn::SendTo(std::string& strMsg)
{
	m_handle.write(strMsg.c_str(), strMsg.length(), std::bind(&TcpConn::OnWriteFinished, this, std::placeholders::_1));
}

void TcpConn::OnWriteFinished(uv::Error error)
{
	if (!error)
	{
		std::cout << error.str() << std::endl;
	}
}
