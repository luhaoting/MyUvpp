#pragma once

#include <list>
#include <memory>

#include "tcp.hpp"
#include "loop.hpp"

using namespace uv;
using namespace std;

class Client final
{
public:
	Client();
	~Client();

	//×èÖ¹¿½±´
	Client(const Client&) = delete;
	Client& operator = (const Client&) = delete;

public: 
	void ConnectTo(string strIp, int nPort);
	void Send(string data);
private: 
	void onConnected(Error error);
	void onRecv(const char* buff, size_t len);
private:
	unique_ptr<Loop> m_base;
	unique_ptr<Tcp> m_handle;
};