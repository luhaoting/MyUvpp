#pragma once

#include "callback.hpp"
#include "free_handle.hpp"
#include "error.hpp"

namespace uv
{
	template<typename handle_T>
	class Handle
	{
	protected:
		Handle():
			m_uv_handle(new handle_T())
			,m_will_close(false)
		{
			assert(m_uv_handle);
			m_uv_handle->data = new callbacks();	//����д����һ��Լ��ģ��ķ���. 2017/1/20
													//���ص�ָ������ ���� 2017/2/23
			assert(m_uv_handle->data);
		}

		//�ƶ����캯��
		Handle(handle_T&& other) :
			m_uv_handle(other.m_uv_handle),
			m_will_close(other.m_will_close)
		{
			other.m_uv_handle = nullptr;   
			other.m_will_close = false;
		}

		Handle& operator= (Handle&& other)
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

		virtual ~Handle()
		{
			if (!m_will_close) //����Ҳ��ª
			{
				free_handle(&m_uv_handle);
			}
		}

	public:
		template<typename T = handle_T>
		T* getHandle()
		{
			return reinterpret_cast<T*>(m_uv_handle);
		}

		template<typename T = handle_T>
		const T* getHandle() const
		{
			return reinterpret_cast<const T*>(m_uv_handle);
		}

		bool is_active() const
		{
			return uv_is_active(reinterpret_cast<const uv_handle_t*>(m_uv_handle)) != 0;
		}

		void close(Callback cb = [] {}) //�رյ�Ĭ�ϻص�Ϊ��
		{
			if(uv_is_closing(getHandle<uv_handle_t>()))
			{
				//��ֹ��ιر�
				return;
			}
			
			callbacks::store(getHandle()->data, internal::eUvCallbackIdClose, cb);// �ص�ѹ��ṹ
			m_will_close = true;
			
			//����
			uv_close(getHandle<uv_handle_t>(),
				[](uv_handle_t* h)
			{
				callbacks::invoke<decltype(cb)>(h->data, internal::eUvCallbackIdClose);
				free_handle(&h);
			}
			);
		}

	protected:
		handle_T* m_uv_handle;
		bool m_will_close;
	};
}