#pragma once

#include <vector>
#include <assert.h>
#include <functional>
#include <memory>
#include "error.hpp"

namespace uv
{
    typedef std::function<void()> Callback;
    typedef std::function<void(Error)> CallbackWithResult;

    namespace internal
    {
        enum EUVCallbackId
        {
            eUvCallbackIdClose = 0,
            eUVCallbackIdListen,
            eUVCallbackIdReadStart,
            eUVCallbackIdWrite,
            eUVCallbackIdShutdown,
            eUVCallbackIdConnect,
            eUVCallbackIdConnect6,
            eUVCallbackIdAccept,

            eUVCallbackIdTcp,
            eUVCallbackIdTimer,

            
            eUVCallbackIdEnd,
        };

        //回调基类
        class callback_object_base
        {
        public :
            callback_object_base(void* data):
                m_data(data)
            {}

            callback_object_base(callback_object_base&&) = default;    //移动构造函数  param 右值引用
            callback_object_base& operator=(callback_object_base&&) = default;

            callback_object_base(const callback_object_base&) = default;    //拷贝构造函数
            callback_object_base& operator=(const callback_object_base&) = default;

            virtual ~callback_object_base(){}

            void* get_data() const    //这里的const 自己写的时候没加。。
            {
                return m_data;
            }

        private:
            void* m_data;
        };

        /*
        *    这里封装了 一个callback类型和args类型的 公共接口
        *    返回值是根据  callback(args) 推导类型。
        *    invoke()
        */
        template<typename callback_T>
        class callback_object : public callback_object_base
        {
        public:
            callback_object(const callback_T& callback, void* data = nullptr)
                : callback_object_base(data)
                , m_callback(callback)
            {}

            /*
            *   这里的 参数入参类型为右值引用。
            *    右值引用类型入参 作用：无论函数体做了什么修改 ,不会影响 args的值。(个人见解)
            */
            template <typename ...arg_T>
            typename std::result_of <callback_T(arg_T...)>::type invoke(arg_T&&... args)
            {
                //m_callback(std::forward<...arg_T>(args));  这里是第一感觉自己的写法
                m_callback(std::forward<arg_T>(args)...); //std::forward 可以帮助推导参数类型
            }

        private:
            callback_T m_callback;
        };
    }

    typedef std::unique_ptr<internal::callback_object_base> callback_object_ptr;

    /*
    *  这个类 存储 callback_object 不同的 callback_T 和args_T组成的 特殊类型
    */
    class callbacks
    {
    public:
        callbacks():
            m_lut(internal::eUVCallbackIdEnd) //这里uv_cid_max 确保任何类型都可以装载
        {}

        callbacks(int max):
            m_lut(max)
        {}

        //将相应的方法 和 参数 绑定到实例
        template<typename callback_T>
        static void store(void* target, int cid, const callback_T& callback, void* data = nullptr)
        {
            reinterpret_cast<callbacks *>(target)->m_lut[cid] = \
                callback_object_ptr(new internal::callback_object<callback_T>(callback, data));
        }

        template<typename callback_T>
        static void* get_data(void* target, int cid)
        {
            return reinterpret_cast<callbacks *>(target)->m_lut[cid]->get_data();
        }

        //个人觉得 这个函数 是这个动态绑定回调的 基石。
        template<typename callback_T, typename ...arg_T>
        static typename std::result_of<callback_T(arg_T...)>::type invoke(void* target, int cid, arg_T&&... args)
        {
            //获取unique_ptr里面的callback_object_base类
            auto dest = dynamic_cast<internal::callback_object<callback_T>*>(\
                reinterpret_cast<callbacks*>(target)->m_lut[cid].get());
            assert(dest);
            return dest->invoke(std::forward<arg_T>(args)...);
        }

    private:
        std::vector<callback_object_ptr> m_lut;
    };
}