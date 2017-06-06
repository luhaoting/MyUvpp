#pragma once

#include <list>
#include <map>
#include <memory>
#include <iostream>
#include <thread>

#include "tcp.hpp"
#include "loop.hpp"

#include "TcpConn.h"

using ID = int64_t;

using namespace uv;
using namespace std;


class ClientCtx : public TcpConn
{
public:
    ClientCtx(uv::Loop& loop)
        :TcpConn(loop)
    {}

    virtual void OnRecv(const char *buff, ssize_t len)
    {
        if (len > 0)
        {
            //这里应有解包
            std::string str;
            str.append(buff, len);

            //free(buff);
            std::cout << "[Client No." << m_id <<" Say]:" << str << std::endl;
            return;
        }
        else
        {
            m_handle.close(mf_close_cb);
        }
    }
};


class CSimpleSvr : public TcpConn
{
public:
    CSimpleSvr(uv::Loop& loop);

    CSimpleSvr(const CSimpleSvr&) = delete;
    CSimpleSvr& operator = (const CSimpleSvr&) = delete;
    virtual ~CSimpleSvr() = default;

    void start(std::string& strIp, int nPort);
    void on_new_connected(uv::Error state);
    int alloc_client_id();
protected:
    virtual void OnRecv(const char *buff, ssize_t len);
private:
    std::map<ID, std::unique_ptr<ClientCtx> >m_all_Client;
};

//class CSimpleClt
//{
//public:
//	CSimpleClt(uv::Loop& loop)
//		:m_rLoop(loop),
//		m_MyHandle(m_rLoop)
//	{
//	}
//
//	bool Connect(const string& strIp, int nProt)
//	{
//		return m_MyHandle.connect(strIp, nProt,
//			[this](uv::Error state)
//		{
//			m_MyHandle.write("i m client", [](uv::Error state) {});
//		}
//		);
//		m_rLoop.run();
//	}
//
//private:
//	uv::Loop& m_rLoop;
//	uv::Tcp m_MyHandle;
//};
