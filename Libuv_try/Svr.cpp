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

	m_listener->listen(std::bind(&Gate::on_new_connection, this, placeholders::_1));
	//this作为参数
	// std::bind  在绑定成员函数是，需要指明调用者
	//
	//placeholders::_1 作为参数
	//  用法举例
	// 	void function(arg1, arg2, arg3, arg4, arg5)
	// 	{
	// 		//do something
	// 	}
	//
	// 	auto g = bind(function, a, b, _2, c, _1);
	//
	// 	新的函数对象：g
	// 		被调用函数：function
	// 		当调用函数对象g时候，函数对象g会调用function函数，并把其参数传给function函数，g的第一个参数会传给function的持有占位符_1的位置，即arg5。第二个参数会传给function的持有占位符_2的位置，即arg3。
	// 		void g(X, Y);
	// 	相对于调用下面函数
	// 		function(function, a, b, Y, c, X);

	return true;
}

void Gate::on_new_connection(uv::Error error)
{
	if (error)
	{
		fprintf(stderr, "on_new_connection error %s\n", error.str()); 
		return;
	}
	
	std::unique_ptr<Tcp> client(new Tcp(*m_base));
	m_listener->accept(*client);
	client->read_start([&](const char *buff, size_t len)
	{
		//接受数据
	}
	);
}