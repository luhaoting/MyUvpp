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
    //this��Ϊ����
    // std::bind  �ڰ󶨳�Ա�����ǣ���Ҫָ��������
    //
    //placeholders::_1 ��Ϊ����
    //  �÷�����
    //     void function(arg1, arg2, arg3, arg4, arg5)
    //     {
    //         //do something
    //     }
    //
    //     auto g = bind(function, a, b, _2, c, _1);
    //
    //     �µĺ�������g
    //         �����ú�����function
    //         �����ú�������gʱ�򣬺�������g�����function�������������������function������g�ĵ�һ�������ᴫ��function�ĳ���ռλ��_1��λ�ã���arg5���ڶ��������ᴫ��function�ĳ���ռλ��_2��λ�ã���arg3��
    //         void g(X, Y);
    //     ����ڵ������溯��
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
        //��������
        
    }
    );
}

std::ostream& G_LOG()
{
    return std::cout;
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
    [var]��ʾֵ���ݲ�׽var
    [=]ֵ���ݲ��������������еı�����
    [&var]���÷�ʽ����var
    [&]���÷�ʽ���������������еı�����
    [this]ֵ���ݲ���ǰ��thisָ�롣
    */
    ID clientid = client_conn->m_id;
    auto close_cb = [this, clientid]()
    {
        m_all_Client.erase(clientid);
    };//���д���ǻ����clientid����ʱ����

    client_conn->m_tcphandel.close(close_cb);//client �رյ�ʱ�� ȥ���Լ��ڷ������ľ��

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

    client_conn->set_write_cb(do_write);//client�����Լ���д�¼�


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

    client_conn->set_read_cb(read_cb);
}

int CSimpleSvr::AllocID()
{
    static int i = 0;
    return i++;
}
