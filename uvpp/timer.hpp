#pragma once

#include "handle.hpp"
#include "loop.hpp"
#include "error.hpp"

#include <chrono>	//时间相关

namespace uv {

	class Timer : public Handle<uv_timer_t>
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

		Error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout, const std::chrono::duration<uint64_t, std::milli> &repeat)
		{
			//封装方法
			callbacks::store(get()->data, internal::eUVCallbackIdTimer, callback);
			//调用方法
			return Error(uv_timer_start(get(), [](uv_timer_t* Handle) {callbacks::invoke<decltype(callback)>(Handle->data, internal::eUVCallbackIdTimer); },
				timeout.count(),
				repeat.count()));
		}

		Error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout)
		{
			callbacks::store(get()->data, internal::eUVCallbackIdTimer, callback);
			return Error(uv_timer_start(get(), 
						[](uv_timer_t* Handle) {callbacks::invoke<decltype(callback)>(Handle->data, internal::eUVCallbackIdTimer); },
						timeout.count(), 0));
		}

		Error stop()
		{
			return Error(uv_timer_stop(get()));
		}

		Error again()
		{
			return Error(uv_timer_again(get()));
		}
	};
}