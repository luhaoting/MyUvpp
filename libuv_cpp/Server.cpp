#include "Server.hpp"
#include <uv.h>
#include "handle.hpp"

//////////////////////////////////////////////////////////////////////////
std::ostream& G_LOG()
{
    return std::cout;
}

CSimpleSvr::CSimpleSvr(uv::Loop& loop)
    :TcpConn(loop)
{
}

void CSimpleSvr::start(std::string& strIp, int nPort)
{
    m_handle.bind(strIp, nPort);
    m_handle.listen(std::bind(&CSimpleSvr::on_new_connected, this, placeholders::_1), 128);
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

    if (m_handle.accept(client_conn->m_handle))
    {
        //init client from accept
        client_conn->m_id = alloc_client_id();
        client_conn->SetReadCb();
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

    client_conn->SetCloseCb(close_cb);

    client_conn->SendTo(string("Connected!"));
    m_all_Client.insert(std::pair<ID, std::unique_ptr<ClientCtx>>(client_conn->m_id, move(client_conn)));
}

int CSimpleSvr::alloc_client_id()
{
    static int i = 0;
    return i++;
}

void CSimpleSvr::OnRecv(const char * buff, ssize_t len)
{
    if (len > 0)
    {
        //这里应有解包
        std::string str;
        str.append(buff, len);

        //free(buff);
        std::cout << "[Server recv data]:" << str << std::endl;
        return;
    }
    else
    {
        m_handle.close(mf_close_cb);
    }
}
