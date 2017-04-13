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