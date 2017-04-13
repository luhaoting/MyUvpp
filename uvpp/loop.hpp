#pragma once

#include "error.hpp"

#include <memory>
#include <functional>

namespace uv {

    /*
    uv_loop 的单例
    */
    class Loop
    {
    public:
        Loop()
            :m_uv_loop(uv_default_loop())
        {
            /*if (!default_loop && uv_loop_init(m_uv_loop.get()))
            {
                throw std::runtime_error("uv_loop_init error");
            }*/

            if (uv_loop_init(m_uv_loop))
            {
                throw std::runtime_error("uv_loop_init error");
            }
        }

        ~Loop()
        {
            //Remember to de - initialize the loop using uv_loop_close(uv_loop_t *) and then delete the storage.
            // no matter default loop or not: http://nikhilm.github.io/uvbook/basics.html#event-loops

            //uv_loop_close 会清理一些这个库自己申请的一些内存。
            if (m_uv_loop)
            {
                /*uv_loop_close(m_uv_loop.get());*/
                uv_loop_close(m_uv_loop);
                m_uv_loop = nullptr;
            }
        }

        //禁止一切拷贝的入口
        Loop(const Loop&) = delete;
        Loop& operator=(const Loop&) = delete;
        
        //右值的优化 在c11疯狂的使用
        Loop(Loop&& other)
            :m_uv_loop(std::forward<decltype(other.m_uv_loop)>(other.m_uv_loop)) //类型推导还可以用在这个地方啊！
        {

        }

        Loop& operator=(Loop&& other)
        {
            if (this != &other)
            {
                m_uv_loop = std::forward<decltype(other.m_uv_loop)>(other.m_uv_loop);
            }
            return *this;
        }

        /*
        * 返回功能句柄 
        */
        uv_loop_t* get()
        {
            return m_uv_loop;
        }

        /*
        *开始运行
        */
        bool run()
        {
            return uv_run(m_uv_loop, UV_RUN_DEFAULT) == 0;
        }

        bool run_once()
        {
            return uv_run(m_uv_loop, UV_RUN_ONCE) == 0;
        }

        /*
        *目前理解 每次循环 的now时间的更新
        */
        void update_time()
        {
            uv_update_time(m_uv_loop);
        }

        int64_t now()
        {
            return uv_now(m_uv_loop);
        }

        void stop()
        {
            uv_stop(m_uv_loop);
        }

    private:

        // Custom deleter
        /*typedef std::function<void(uv_loop_t*)> Deleter;
        void destroy(uv_loop_t *loop) const
        {
            if (!default_loop)
            {
                delete loop;
            }
        }
*/
        bool default_loop;
        //std::unique_ptr<uv_loop_t, Deleter> m_uv_loop; //这个写法传入 清理函数
        uv_loop_t* m_uv_loop;
    };


    /**
    *  Starts the default loop.
    */
    inline int run()
    {
        return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    }

    /**
    *  Polls for new events without blocking for the default loop.
    */
    inline int run_once()
    {
        return uv_run(uv_default_loop(), UV_RUN_ONCE);
    }
}