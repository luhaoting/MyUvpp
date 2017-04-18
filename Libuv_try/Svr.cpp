#include "Svr.hpp"

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
    //     void function(arg1, arg2, arg3, arg4, arg5)
    //     {
    //         //do something
    //     }
    //
    //     auto g = bind(function, a, b, _2, c, _1);
    //
    //     新的函数对象：g
    //         被调用函数：function
    //         当调用函数对象g时候，函数对象g会调用function函数，并把其参数传给function函数，g的第一个参数会传给function的持有占位符_1的位置，即arg5。第二个参数会传给function的持有占位符_2的位置，即arg3。
    //         void g(X, Y);
    //     相对于调用下面函数
    //         function(function, a, b, Y, c, X);

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






void CSimpleSvr::start(std::string& strIp, int nPort)
{
    m_SvrHandle.bind(strIp, nPort);
    m_SvrHandle.listen(std::bind(&CSimpleSvr::on_new_connected, this, placeholders::_1), 128);
}

void CSimpleSvr::on_new_connected(uv::Error state)
{
    if (state)
    {
        G_LOG() << "Client Connected Error UvError:" << state.str() << std::endl;
    }

    //alloc client
    auto client_conn = std::make_unique<ClientCtx>(m_rLoop);

    if (m_SvrHandle.accept(client_conn->m_tcphandel))
    {
        //init client from accept
        client_conn->m_id = AllocID();
        m_all_Client.insert(std::pair<ID, std::unique_ptr<ClientCtx>>(client_conn->m_id, move(client_conn)));
    }
    else
    {
        G_LOG() << "Client Accept Error" << std::endl;
        return;
    }
    /*
    [var]表示值传递捕捉var
    [=]值传递捕获父作用域中所有的变量。
    [&var]引用方式捕获var
    [&]引用方式捕获父作用域中所有的变量。
    [this]值传递捕获当前的this指针。
    */
    ID clientid = client_conn->m_id;
    auto close_cb = [this, clientid]()
    {
        m_all_Client.erase(clientid);
    };//这个写法是会产生clientid个临时变量

    client_conn->m_tcphandel.close(close_cb);//client 关闭的时候 去除自己在服务器的句柄

    auto write_cb = [&](uv::Error error)
    {
        if (!error)
        {
            //TODO
        }
        else
        {
            G_LOG() << "TCP client write error: ID = " << client_conn->m_id << std::endl;
            client_conn->m_tcphandel.close(close_cb);
        }
    };
}

int CSimpleSvr::AllocID()
{
    static int i = 0;
    return i++;
}
