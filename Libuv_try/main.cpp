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

#define uv_work 1

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
            //fprintf(stdout,"���͵�����Ϊ:\n");
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