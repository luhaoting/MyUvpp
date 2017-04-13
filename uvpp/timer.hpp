#pragma once

#include "handle.hpp"
#include "loop.hpp"
#include "error.hpp"

#include <chrono>    //ʱ�����

namespace uv {

    class Timer : public Handle<uv_timer_t>
    {
    public:
        Timer() :
            Handle<uv_timer_t>()
        {
            uv_timer_init(uv_default_loop(), get());
        }

        Timer(Loop& l) :
            Handle<uv_timer_t>()
        {
            uv_timer_init(l.get(), get());
        }

        Error start(std::function<void()> callback, const std::chrono::duration<uint64_t, std::milli>& timeout, const std::chrono::duration<uint64_t, std::milli> &repeat)
        {
            //��װ����
            callbacks::store(get()->data, internal::eUVCallbackIdTimer, callback);
            //���÷���
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