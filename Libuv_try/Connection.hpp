// **********************************************************************
// Copyright (c) 2016 All Rights Reserved
// File     : Connection.cs
// Author   : luhaoting
// Created  : 2017/04/11
// Porpuse  : 
// **********************************************************************

#pragma once
namespace CS
{

	class ConnectCtx
	{
		//链接的用户数据已经回调处理
	};

	class Connection
	{
	public:
		Connection()
		{
			
		}

		virtual ~Connection() = default;

		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;
		Connection(Connection&&) = default;
		Connection& operator=(Connection&&) = default;

		ConnectCtx

	};
}
