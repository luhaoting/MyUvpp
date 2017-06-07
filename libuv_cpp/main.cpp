#include <stdio.h>
#include <uv.h>
#include <malloc.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <map>

/*
#pragma comment(lib,"Ws2_32.lib") 
#pragma comment(lib,"Psapi.lib") 
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"Userenv.lib") 
����Ŀ���Ҫ�ֶ��� ���˹���

#pragma comment(lib,"libuv.lib") 
*/


#include "loop.hpp"
#include "timer.hpp"
#include "tcp.hpp"

#define classsvr 1

#if uv_work
static void PrintPoint(uv_timer_t* t)
{
    std::cout << " ." ;
}

static void PrintPoint1()
{
    std::cout << " .";
}

int getVal()
{
    int o = 2333;
    return o;
}

template <typename T>
void g(T& val)
{
    std::cout << val << std::endl;
    val = 99;
}


int main()
{
    //int&& rVal = getVal();    //��ֵ����
    //int i = rVal;            //��ֵ���ÿ��Խ�ס��ʱ�����������ơ�
    //int ci = i;
    //g(ci);
    //std::cout << ci << std::endl;
    //std::cout << i << std::endl;

    //uv_loop_t *loop = uv_default_loop();
    //uv_timer_t *timer = new uv_timer_t();
    //uv_timer_init(loop, timer);                            //��ʼ����ʱ��
    //uv_timer_start(timer, PrintPoint, 10000, /*1000*/0);    //�趨10s��ʱ��,ÿ1s�ظ�һ��
    ////uv_timer_set_repeat(&timer, 1000);                    //�趨�ζ�ʱ��ִ��֮��,ÿ1s�ظ�һ��
    //
    //uv_run(loop, UV_RUN_DEFAULT);
    //free(timer);  
    //uv_loop_close(loop);
    //free(loop);
    //timer 


    //  int uv_loop_close(uv_loop_t* loop)
    //  �ͷ����е�ѭ���ڲ�����Դ��ֻ����loop������в������д򿪵�handle�Լ�requests�Ѿ����رգ�
    //    ����᷵��UV_EBUSY��������������֮���û������ͷ�Ϊloop������ڴ���

    //���ֹ��췽ʽ ����uv-common.c line��635 ������ԭ��handle��loop֮ǰ��free loop�����ٴ������� 
    //uv::Loop loop;
    //uv::Timer timer;
    //����handle::close();�Ͳ�����


    uv::Loop loop;

    uv::Timer timer(loop);
    uv::Tcp tcp(loop);

    tcp.bind("127.0.0.1", 9123);

    std::chrono::duration<int, std::milli> time(1000);
    timer.start([]() {std::cout << " ." << std::endl; }, time);
    loop.run();

    timer.close();
    tcp.close();
    return 0;
}
#endif



typedef struct write_req_t
{
    uv_write_t req;
    uv_buf_t buf;
}write_req_t;


#if mysvr
#include <uv.h>
uv_loop_t *loop = uv_default_loop();

void client_close(uv_handle_t* handle)
{
    free(handle);
}

void on_new_connection(uv_stream_t *server, int status) 
{
    if (status)
    {
        std::cout << uv::Error(status).str() << std::endl;
        return;
    }

    auto alloc_cb = [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {
        //����Ӧ�ô�һ���ڴ��������
        *buf = uv_buf_init(new char[suggested_size], suggested_size);
    };

    auto read_cb = [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        if (nread < 0)
        {
            uv_close((uv_handle_t*)stream, client_close);
        }
        else
        {
            //����Ӧ�н��
            std::string str;
            str.append(buf->base, nread);
            free(buf->base);
            std::cout << str << std::endl;
        }
    };

    uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    
    //client->data = server;
    if (uv_accept(server, (uv_stream_t*)client) == 0) 
    {
        uv_read_start((uv_stream_t*)client, alloc_cb, read_cb);
    }
    else 
    {
        uv_close((uv_handle_t*)client, NULL);
    }
}

int main()
{
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    int ret = uv_ip4_addr("127.0.0.1", 95527, &bind_addr);
    ret = uv_tcp_bind(&server, reinterpret_cast<sockaddr*>(&bind_addr), 0);
    ret = uv_listen((uv_stream_t*)&server, 128, on_new_connection);

    if (ret < 0) 
    {
        fprintf(stderr, "Listen error %s\n", uv::Error(ret));
        return 1;
    }

    uv_run(loop, UV_RUN_DEFAULT);
    system("pause");
}

#endif

#if myclt

void disconnect(uv_handle_t* handle)
{
    free(handle);
}

void on_connect(uv_connect_t* req, int status)
{
    if (status)
    {
        std::cout << Error(status).str() << std::endl;
    }

    write_req_t* wq = (write_req_t*)malloc(sizeof(write_req_t));

    std::string *msg = new string("i have connected !");
    wq->buf = uv_buf_init(const_cast<char*>(msg->c_str()), msg->size());

    uv_write(&wq->req, req->handle, &wq->buf, 1,
        [](uv_write_t* req, int status)
    {
        free(reinterpret_cast<write_req_t*>(req)->buf.base);
        free(req);
    });
}

int main(int argc, char* argv[])
{
    uv_tcp_t socket;
    uv_tcp_init(loop, &socket);

    uv_connect_t connect;

    struct sockaddr_in dest;
    int ret = uv_ip4_addr("127.0.0.1", 95527, &dest);

    ret = uv_tcp_connect(&connect, &socket, (const struct sockaddr*)(&dest), on_connect);

    ret = uv_run(loop, UV_RUN_DEFAULT);
    system("pause");
}

#endif

#if classsvr 
#include "Server.hpp"

int main(int argc, char* argv[])
{
    uv::Loop base;
    CSimpleSvr Svr(base);
    
    Svr.start(string("127.0.0.1"), 95527);
    return 0;
}

#endif


#if classclt
#include <functional>
#include "Client.h"


int main(int argc, char* argv[])
{
    uv::Loop base;
    Client client(base);
    client.Start("127.0.0.1", 95527);
}

#endif