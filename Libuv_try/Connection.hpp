// **********************************************************************
// Copyright (c) 2016 All Rights Reserved
// File     : Connection.cs
// Author   : luhaoting
// Created  : 2017/04/11
// Porpuse  : 
// **********************************************************************

#pragma once
#include "tcp.hpp"
#define BUFFER_SIZE 1024

using namespace uv;

namespace cs
{
    //链接的用户数据和处理的回调
    class Protocol
    {
    public:
        Protocol()
        {
            read_buf_.base = (char*)malloc(BUFFER_SIZE);
            read_buf_.len = BUFFER_SIZE;
            write_req.data = this;          //store self
        }

        ~Protocol()
        {
            free(read_buf_.base);
        }
        
        uv_tcp_t client;                            //data filed store this
        int clientid;

        uv_write_t write_req;                       //store this on data
        uv_buf_t read_buf_;
    };//暴露了太多的底层类型了

    class Connection
    {
    public:
        Connection()
        {
            m_protocol = std::make_unique<Protocol>();
        }

        virtual ~Connection()                   = default;

        Connection(const Connection&)                   = delete;
        Connection& operator=(const Connection&)        = delete;
        Connection(Connection&&)                = default;
        Connection& operator=(Connection&&)     = default;

        std::unique_ptr<Protocol> m_protocol;                        //用户数据，回调信息应该都在这里
    };
}
