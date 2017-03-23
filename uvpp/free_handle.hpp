#pragma once

#include "callback.hpp"

namespace uv
{
	template <typename handle_T>
	inline void free_handle(handle_T ** h)
	{
		if (*h == nullptr)
		{
			return;
		}

		if ((*h)->data)
		{
			delete reinterpret_cast<callbacks *>((*h)->data);
			(*h)->data = nullptr;
		}

		//这里处理的很丑陋 ,但是这是 C/C++ 的臭味没有反射
		switch ((*h)->type)
		{
		case UV_TCP:
			delete reinterpret_cast<uv_tcp_t*>(*h);
			break;

		case UV_UDP:
			delete reinterpret_cast<uv_udp_t*>(*h);
			break;

		case UV_NAMED_PIPE:
			delete reinterpret_cast<uv_pipe_t*>(*h);
			break;

		case UV_TTY:
			delete reinterpret_cast<uv_tty_t*>(*h);
			break;

		case UV_TIMER:
			delete reinterpret_cast<uv_timer_t*>(*h);
			break;

		case UV_SIGNAL:
			delete reinterpret_cast<uv_signal_t*>(*h);
			break;

		case UV_POLL:
			delete reinterpret_cast<uv_poll_t*>(*h);
			break;

		case UV_ASYNC:
			delete reinterpret_cast<uv_async_t*>(*h);
			break;

		case UV_IDLE:
			delete reinterpret_cast<uv_idle_t*>(*h);
			break;

		case UV_FS_EVENT:
			delete reinterpret_cast<uv_fs_event_t*>(*h);
			break;

		default:
			assert(0);
			throw std::runtime_error("free_handle can't handle this type");
			break;
			*h = nullptr;
		}
	}
}