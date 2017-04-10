#include "Client.h"

Client::Client() :
	m_base(new Loop()),
	m_handle(new Tcp(*m_base))
{
}

Client::~Client()
{
}


void Client::ConnectTo(string strIp, int nPort)
{
// 	TRY
// 
// 		if (!m_pSocket->connectTo(server, unPort))
// 		{
// 			return false;
// 		}

	m_handle->connect(strIp, nPort, std::bind(&Client::onConnected, this, placeholders::_1));

}

void Client::onConnected(Error error)
{
	// 	m_pSocket->setnonblock();
	// 
	// 	m_pevBuff = bufferevent_new(m_pSocket->m_nSocket, OnReceive, NULL, OnEv, this);
	// 	bufferevent_base_set(m_pEventBase, m_pevBuff);
	// 	bufferevent_enable(m_pevBuff, EV_READ | EV_PERSIST);
	// 	bufferevent_priority_set(m_pevBuff, 2);//3 is middle pri, so 2 is handle first than 3
	// 	return true;

	m_handle->read_start(std::bind(&Client::onRecv, this, placeholders::_1, placeholders::_2));
}


void Client::Send(string data)
{
	
}

void Client::onRecv(const char* buff, size_t len)
{

}

