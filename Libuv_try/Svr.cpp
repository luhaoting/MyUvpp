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
//////////////////////////////////////////////////////////////////////////
std::ostream& G_LOG()
{
    return std::cout;
}

void CSimpleSvr::start(std::string& strIp, int nPort)
{
    m_SvrHandle.bind(strIp, nPort);
    m_SvrHandle.listen(std::bind(&CSimpleSvr::on_new_connected, this, placeholders::_1), 128);
    m_rLoop.run();
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
        client_conn->m_id = alloc_client_id();
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
    ID client_id = client_conn->m_id;
    auto close_cb = [this, client_id]()
    {
        m_all_Client.erase(client_id);
    };

    client_conn->m_tcphandel.close(close_cb);//client 关闭的时候 去除自己在服务器的句柄

    auto write_cb = [&](uv::Error error)
    {
        if (!error)
        {
            //TODO
            client_conn->on_write_finished();
        }
        else
        {
            G_LOG() << "TCP client write error: ID = " << client_conn->m_id << std::endl;
            client_conn->m_tcphandel.close(close_cb);
        }
    };

    auto do_write = [&](const char* buff, size_t len)
    {
        client_conn->m_tcphandel.write(buff, len, write_cb);
    };

    auto read_cb = [&](const char *buff, ssize_t len) 
    {
        if (len < 0)
        {
            cerr << "TCP client read error: ID = " << client_conn->m_id << std::endl;
            client_conn->m_tcphandel.close(close_cb);
        }
        else
        {
            std::string msg;
            msg.append(buff, len);
            G_LOG() << "client ID = " << client_conn->m_id << "recv [" << msg << "]" << std::endl;
        }
    };

    client_conn->set_write_cb(do_write);//client设置自己的写事件
    client_conn->set_read_cb(read_cb);

    m_all_Client.insert(std::pair<ID, std::unique_ptr<ClientCtx>>(client_conn->m_id, move(client_conn)));
}

int CSimpleSvr::alloc_client_id()
{
    static int i = 0;
    return i++;
}


void CSimpleSvr::start_tread(void* self)
{
    CSimpleSvr* svr = (CSimpleSvr*)self;
    svr->m_rLoop.run();
}