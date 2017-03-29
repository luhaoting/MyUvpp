#include "Svr.h"

Gate::Gate() :
	m_base(new Loop()),
	m_listener(new Tcp(*m_base.get()))
{
}

Gate::~Gate()
{
}

bool Gate::listen(const int nPort)
{
	m_listener->listen()
}
