// **********************************************************************
// Copyright (c) 2016 All Rights Reserved
// File     : tcpcon_ctx.cs
// Author   : luhaoting
// Created  : 2017/04/12
// Porpuse  : 
// **********************************************************************

#pragma  once
#include <string>
#include <list>
#include "uv.h"
#include "packet_sync.h"
#include "pod_circularbuffer.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024*10)
#endif

namespace uv
{

	typedef struct _tcpclient_ctx {
		uv_tcp_t tcphandle;//store this on data
		uv_write_t write_req;//store this on data
		PacketSync* packet_;//store this on userdata
		uv_buf_t read_buf_;
		int clientid;
		void* parent_server;//store TCPClient point
	} TcpClientCtx;
	TcpClientCtx* AllocTcpClientCtx(void* parentserver);
	void FreeTcpClientCtx(TcpClientCtx* ctx);

	typedef struct _write_param {//param of uv_write
		uv_write_t write_req_;//store TCPClient on data
		uv_buf_t buf_;
		int buf_truelen_;
	}write_param;
	write_param * AllocWriteParam(void);
	void FreeWriteParam(write_param* param);
}