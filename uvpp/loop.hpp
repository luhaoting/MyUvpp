#pragma once

#include "error.hpp"

#include <memory>
#include <functional>

namespace uv {

    /*
    uv_loop �ĵ���
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

            //uv_loop_close ������һЩ������Լ������һЩ�ڴ档
            if (m_uv_loop)
            {
                /*uv_loop_close(m_uv_loop.get());*/
                uv_loop_close(m_uv_loop);
                m_uv_loop = nullptr;
            }
        }

        //��ֹһ�п��������
        Loop(const Loop&) = delete;
        Loop& operator=(const Loop&) = delete;
        
        //��ֵ���Ż� ��c11����ʹ��
        Loop(Loop&& other)
            :m_uv_loop(std::forward<decltype(other.m_uv_loop)>(other.m_uv_loop)) //�����Ƶ���������������ط�����
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
        * ���ع��ܾ�� 
        */
        uv_loop_t* get()
        {
            return m_uv_loop;
        }

        /*
        *��ʼ����
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
        *Ŀǰ��� ÿ��ѭ�� ��nowʱ��ĸ���
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
        //std::unique_ptr<uv_loop_t, Deleter> m_uv_loop; //���д������ ������
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