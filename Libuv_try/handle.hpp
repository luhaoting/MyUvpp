#pragma once

#include "callback.hpp"
#include "free_handle.hpp"
#include "error.hpp"

namespace uv
{
	template<typename handle_T>
	class handle
	{
	protected:
		handle():
			m_uv_handle(new handle_T())
			,m_will_close(false)
		{
			assert(m_uv_handle);
			m_uv_handle->data = new callbacks();	//这种写法是一种约束模板的方法. 2017/1/20
													//将回调指针类型 放入 2017/2/23
			assert(m_uv_handle->data);
		}

		//移动构造函数
		handle(handle_T&& other) :
			m_uv_handle(other.m_uv_handle),
			m_will_close(other.m_will_close)
		{
			other.m_uv_handle = nullptr;   
			other.m_will_close = false;
		}

		handle& operator= (handle&& other)
		{
			if (this == &other)
			{
				return *this;
			}

			m_uv_handle = other.m_uv_handle;
			m_will_close = other.m_will_close;
			other.m_uv_handle = nullptr;
			other.m_will_close = false;
			return *this;
		}

		virtual ~handle()
		{
			if (!m_will_close) //这里也丑陋
			{
				free_handle(&m_uv_handle);
			}
		}

	public:
		template<typename T = handle_T>
		T* get()
		{
			return reinterpret_cast<T*>(m_uv_handle);
		}

		template<typename T = handle_T>
		const T* get() const
		{
			return reinterpret_cast<const T*>(m_uv_handle);
		}

		bool is_active() const
		{
			return uv_is_active(reinterpret_cast<const uv_handle_t*>(m_uv_handle)) != 0;
		}

		void close(Callback cb = [] {}) //关闭的默认回调为空
		{
			if(uv_is_closing(get<uv_handle_t>()))
			{
				//防止多次关闭
				return;
			}
			
			callbacks::store(get()->data, internal::uv_cid_close, cb);// 回调压入结构
			m_will_close = true;
			
			//调用
			uv_close(get<uv_handle_t>(),
				[](uv_handle_t* h)
			{
				callbacks::invoke<decltype(cb)>(handle->data, internal::uv_cid_close);
				free_handle(&h);
			}
			);
		}

	protected:
		handle_T* m_uv_handle;
		bool m_will_close;
	};
}