#pragma once

#include "handle.hpp"
#include "loop.hpp"
#include "error.hpp"

#include <chrono>	//时间相关

namespace uv {

	class Timer : public handle<uv_timer_t>
	{
	public:
		//Timer() :
		//	handle<uv_timer_t>()
		//{
		//	uv_timer_init(uv_default_loop(), get());
		//}

		//Timer(loop& l) :
		//	handle<uv_timer_t>()
		//{
		//	uv_timer_init(l.get(), get());
		//}
		void Init(Loop& l)
		{
			uv_timer_init(l.get(), get());
		}

		error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout, const std::chrono::duration<uint64_t, std::milli> &repeat)
		{
			//封装方法
			callbacks::store(get()->data, internal::uv_cid_timer, callback);
			//调用方法
			return error(uv_timer_start(get(), [](uv_timer_t* handle) {callbacks::invoke<decltype(callback)>(handle->data, internal::uv_cid_timer); },
				timeout.count(),
				repeat.count()));
		}

		error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout)
		{
			callbacks::store(get()->data, internal::uv_cid_timer, callback);
			return error(uv_timer_start(get(), 
						[](uv_timer_t* handle) {callbacks::invoke<decltype(callback)>(handle->data, internal::uv_cid_timer); },
						timeout.count(), 0));
		}

		error stop()
		{
			return error(uv_timer_stop(get()));
		}

		error again()
		{
			return error(uv_timer_again(get()));
		}
	};
}