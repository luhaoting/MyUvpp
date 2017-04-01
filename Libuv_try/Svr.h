#pragma once

#include <list>
#include <memory>

#include "tcp.hpp"
#include "loop.hpp"

using namespace uv;
using namespace std;

class Gate final
{
public:
	Gate();
	~Gate();
	
	//×èÖ¹¿½±´
	Gate(const Gate&) = delete;
	Gate& operator = (const Gate&) = delete;

	bool listen(const int nPort);

private:
	void on_new_connection(uv::Error error);
	
private:
	unique_ptr<Loop> m_base;
	unique_ptr<Tcp> m_listener;

	list<unique_ptr<Tcp> > m_active_client;

	
};