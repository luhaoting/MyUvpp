#pragma once

#include "handle.hpp"
#include "loop.hpp"
#include "error.hpp"

#include <chrono>	//时间相关

namespace uv {

	class Timer : public Handle<uv_timer_t>
	{
	public:
		Timer() :
			Handle<uv_timer_t>()
		{
			uv_timer_init(uv_default_loop(), getHandle());
		}

		Timer(Loop& l) :
			Handle<uv_timer_t>()
		{
			uv_timer_init(l.get(), getHandle());
		}

		Error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout, const std::chrono::duration<uint64_t, std::milli> &repeat)
		{
			//封装方法
			callbacks::store(getHandle()->data, internal::eUVCallbackIdTimer, callback);
			//调用方法
			return Error(uv_timer_start(getHandle(), [](uv_timer_t* Handle) {callbacks::invoke<decltype(callback)>(Handle->data, internal::eUVCallbackIdTimer); },
				timeout.count(),
				repeat.count()));
		}

		Error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout)
		{
			callbacks::store(getHandle()->data, internal::eUVCallbackIdTimer, callback);
			return Error(uv_timer_start(getHandle(), 
						[](uv_timer_t* Handle) {callbacks::invoke<decltype(callback)>(Handle->data, internal::eUVCallbackIdTimer); },
						timeout.count(), 0));
		}

		Error stop()
		{
			return Error(uv_timer_stop(getHandle()));
		}

		Error again()
		{
			return Error(uv_timer_again(getHandle()));
		}
	};
}