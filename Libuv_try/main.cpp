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
上面的库需要手动加 日了狗了

#pragma comment(lib,"libuv.lib") 
*/


#include "loop.hpp"
#include "timer.hpp"
#include "tcp.hpp"

#define mysvr 1

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
    //int&& rVal = getVal();    //右值引用
    //int i = rVal;            //右值引用可以接住临时变量而不复制。
    //int ci = i;
    //g(ci);
    //std::cout << ci << std::endl;
    //std::cout << i << std::endl;

    //uv_loop_t *loop = uv_default_loop();
    //uv_timer_t *timer = new uv_timer_t();
    //uv_timer_init(loop, timer);                            //初始化定时器
    //uv_timer_start(timer, PrintPoint, 10000, /*1000*/0);    //设定10s定时器,每1s重复一次
    ////uv_timer_set_repeat(&timer, 1000);                    //设定次定时器执行之后,每1s重复一次
    //
    //uv_run(loop, UV_RUN_DEFAULT);
    //free(timer);  
    //uv_loop_close(loop);
    //free(loop);
    //timer 


    //  int uv_loop_close(uv_loop_t* loop)
    //  释放所有的循环内部的资源。只能在loop完成运行并且所有打开的handle以及requests已经被关闭，
    //    否则会返回UV_EBUSY。当本函数返回之后，用户可以释放为loop分配的内存了

    //这种构造方式 会在uv-common.c line：635 崩溃。原因：handle在loop之前被free loop的销毁处理会出错 
    //uv::Loop loop;
    //uv::Timer timer;
    //加入handle::close();就不会了


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

#if server_work
#include <iostream>
#include <string>
#include "tcpserver.h"
class TestTCPProtocol : public TCPServerProtocolProcess
{
public:
    TestTCPProtocol() {}
    virtual ~TestTCPProtocol() {}
    virtual const std::string& ParsePacket(const NetPacket& packet, const unsigned char* buf)
    {
        static char senddata[256];
        sprintf(senddata, "****recv datalen %d", packet.datalen);
        fprintf(stdout, "%s\n", senddata);

        NetPacket tmppack = packet;
        tmppack.datalen = (std::min)(strlen(senddata), sizeof(senddata) - 1);
        pro_packet_ = PacketData(tmppack, (const unsigned char*)senddata);
        return pro_packet_;
    }
private:
    std::string pro_packet_;
};

using namespace std;
using namespace uv;
bool is_eist = false;
int call_time = 0;

TCPServer server(0x01, 0x02);

void CloseCB(int clientid, void* userdata)
{
    fprintf(stdout, "cliend %d close\n", clientid);
    TCPServer *theclass = (TCPServer *)userdata;
    //is_eist = true;
}

void NewConnect(int clientid, void* userdata)
{
    fprintf(stdout, "new connect:%d\n", clientid);
    server.SetRecvCB(clientid, NULL, NULL);
}


int main()
{
    TestTCPProtocol protocol;

    TCPServer::StartLog("log/");
    server.SetNewConnectCB(NewConnect, &server);
    server.SetPortocol(&protocol);
    if (!server.Start("0.0.0.0", 12345)) {
        fprintf(stdout, "Start Server error:%s\n", server.GetLastErrMsg());
    }
    server.SetKeepAlive(1, 60);//enable Keepalive, 60s
    fprintf(stdout, "server return on main.\n");
    while (!is_eist)
    {
        Sleep(10);
    }
    return 0;
}
#endif

#if client_work
#include <iostream>
#include <string>
#include "tcpclient.h"
//#include "mswin_special/sys/DumpFile.h"
using namespace std;
using namespace uv;

std::string serverip;
int call_time = 0;
bool is_exist = false;

void CloseCB(int clientid, void* userdata)
{
    fprintf(stdout, "cliend %d close\n", clientid);
    TCPClient* client = (TCPClient*)userdata;
    client->Close();
}

void ReadCB(const NetPacket& packet, const unsigned char* buf, void* userdata)
{
    fprintf(stdout, "call time %d\n", ++call_time);
    if (call_time > 5000) {
        return;
    }
    char senddata[256] = { 0 };
    TCPClient* client = (TCPClient*)userdata;
    sprintf(senddata, "****recv server data(%p,%d)", client, packet.datalen);
    fprintf(stdout, "%s\n", senddata);
    NetPacket tmppack = packet;
    tmppack.datalen = (std::min)(strlen(senddata), sizeof(senddata) - 1);
    std::string retstr = PacketData(tmppack, (const unsigned char*)senddata);
    if (client->Send(&retstr[0], retstr.length()) <= 0) {
        fprintf(stdout, "(%p)send error.%s\n", client, client->GetLastErrMsg());
    }
}

int main(int argc, char** argv)
{
//     if (argc != 3) {
//         fprintf(stdout, "usage: %s server_ip_address clientcount\neg.%s 192.168.1.1 50\n", argv[0], argv[0]);
//         return 0;
//     }

    serverip = "127.0.0.1";

    const int clientsize = 2;
    TCPClient** pClients = new TCPClient*[clientsize];
    TCPClient::StartLog("log/");

    int i = 0;
    char senddata[256];
    for (int i = 0; i < clientsize; ++i) {
        pClients[i] = new TCPClient(0x01, 0x02);
        pClients[i]->SetRecvCB(ReadCB, pClients[i]);
        pClients[i]->SetClosedCB(CloseCB, pClients[i]);
        if (!pClients[i]->Connect(serverip.c_str(), 12345)) {
            fprintf(stdout, "connect error:%s\n", pClients[i]->GetLastErrMsg());
        }
        else {
            fprintf(stdout, "client(%p) connect succeed.\n", pClients[i]);
        }
        memset(senddata, 0, sizeof(senddata));
        sprintf(senddata, "client(%p) call %d", pClients[i], ++call_time);
        NetPacket packet;
        packet.header = 0x01;
        packet.tail = 0x02;
        packet.datalen = (std::min)(strlen(senddata), sizeof(senddata) - 1);
        std::string str = PacketData(packet, (const unsigned char*)senddata);
        if (pClients[i]->Send(&str[0], str.length()) <= 0) {
            fprintf(stdout, "(%p)send error.%s\n", pClients[i], pClients[i]->GetLastErrMsg());
        }
        else {
            //fprintf(stdout,"发送的数据为:\n");
            //         for (int i=0; i<str.length(); ++i) {
            //             fprintf(stdout,"%02X",(unsigned char)str[i]);
            //         }
            //         fprintf(stdout,"\n");
            fprintf(stdout, "send succeed:%s\n", senddata);
        }
    }
    while (!is_exist) {
        Sleep(10);
    }
    return 0;
}
#endif


#if cyclebuffer
#include "ring_buffer.hpp"

typedef struct msg
{
    int a;
} Msg;

using namespace uti;

int main(int argc, char* argv[])
{
    ringbuffer<Msg, 1> *ring = new ringbuffer<Msg, 1>();
    Msg *m = new Msg();
    m->a = 1;
    Msg *m1 = new Msg();
    m1->a = 2;

    int capacity = ring->capacity();
    int size = ring->size();
    int maxsize = ring->max_size();

    ring->push(*m);
    int aftercapacity = ring->capacity();
    int aftersize = ring->size();
    int aftermaxsize = ring->max_size();

    ring->pop();

    int popcapacity = ring->capacity();
    int popsize = ring->size();
    int popmaxsize = ring->max_size();

    ring->push_back(*m1);
    int i = ring->max_size();

    Msg re = ring->front();

    return 0;
}

#endif

#if mysvr
#include "Svr.hpp"
#define  myclt
#ifdef myclt
// int main(int argc, char* argv[])
// {
//     uv::Loop loop;
//     CSimpleClt clt(loop);
//     bool ret = clt.Connect("112.168.17.2", 13134);
//     G_LOG() << "connected ret = " << (ret ? "true" : "false") << std::endl;
//     system("pause");
// }
#endif // myclt
// 
// int main(int argc, char* argv[])
// {
//     uv::Loop loop;
//     CSimpleSvr svr(loop);
//     loop.run();
//     svr.start(std::string("127.0.0.1"), 13134);
// }


#include <uv.h>
uv_loop_t *loop = uv_default_loop();

void on_new_connection(uv_stream_t *server, int status) {
    if (status) 
    {
        
        std::cout << Error(status).str() << std::endl;
        return;
    }

    auto alloc_cb = [](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
    {

        *buf = uv_buf_init(new char[suggested_size], suggested_size);
    };

    auto read_cb = [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
        std::string str;
        str.append(buf->base, nread);
        std::cout << str << std::endl;
    };

    uv_tcp_t *client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
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
    if (ret < 0) {
        fprintf(stderr, "Listen error %s\n", Error(ret));
        return 1;
    }
    uv_run(loop, UV_RUN_DEFAULT);
    system("pause");
}


typedef struct write_req_t
{
    uv_write_t req;
    uv_buf_t buf;
}write_req_t;

void on_connect(uv_connect_t* req, int status)
{
    if (status)
    {
        std::cout << Error(status).str() << std::endl;
    }
    std::string msg = "i have connected !";
    write_req_t* wq = (write_req_t*)malloc(sizeof(write_req_t));
    uv_write(&wq->req, req->handle, &wq->buf, 1, 
        [](uv_write_t* req, int status) 
    {
        write_req_t *wr;
        wr = (write_req_t*)req;
        free(wr->buf.base);
        free(wr);
    });
}

// int main(int argc, char* argv[])
// {
//     uv_tcp_t socket;
//     uv_tcp_init(loop, &socket);
// 
//     uv_connect_t connect;
// 
//     struct sockaddr_in dest;
//     int ret = uv_ip4_addr("127.0.0.1", 95527, &dest);
// 
//     ret = uv_tcp_connect(&connect, &socket, (const struct sockaddr*)(&dest), on_connect);
// 
//     ret = uv_run(loop, UV_RUN_DEFAULT);
//     system("pause");
// }

#endif