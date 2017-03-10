#pragma once

#include <vector>
#include <assert.h>
#include <functional>
#include <memory>


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
			eUVCallbackIdConnect,
			eUVCallbackIdAccept,

			eUVCallbackIdTcp,
			eUVCallbackIdTimer,

			eUVCallbackIdEnd,
		};

		//�ص�����
		class callback_object_base
		{
		public :
			callback_object_base(void* data):
				m_data(data)
			{}

			callback_object_base(callback_object_base&&) = default;	//�ƶ����캯��  param ��ֵ����
			callback_object_base& operator=(callback_object_base&&) = default;

			callback_object_base(const callback_object_base&) = default;	//�������캯��
			callback_object_base& operator=(const callback_object_base&) = default;

			virtual ~callback_object_base(){}

			void* get_data() const	//�����const �Լ�д��ʱ��û�ӡ���
			{
				return m_data;
			}

		private:
			void* m_data;
		};

		/*
		*    �����װ�� һ��callback���ͺ�args���͵� �����ӿ�
		*    ����ֵ�Ǹ���  callback(args) �Ƶ����͡�
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
			*   ����� �����������Ϊ��ֵ���á�
			*	��ֵ����������� ���ã����ۺ���������ʲô�޸� ,����Ӱ�� args��ֵ��(���˼���)
			*/
			template <typename ...arg_T>
			typename std::result_of <callback_T(arg_T...)>::type invoke(arg_T&&... args)
			{
				//m_callback(std::forward<...arg_T>(args));  �����ǵ�һ�о��Լ���д��
				m_callback(std::forward<arg_T>(args)...); //std::forward ���԰����Ƶ���������
			}

		private:
			callback_T m_callback;
		};
	}

	typedef std::unique_ptr<internal::callback_object_base> callback_object_ptr;

	/*
	*  ����� �洢 callback_object ��ͬ�� callback_T ��args_T��ɵ� ��������
	*/
	class callbacks
	{
	public:
		callbacks():
			m_lut(internal::eUVCallbackIdEnd) //����uv_cid_max ȷ���κ����Ͷ�����װ��
		{}

		callbacks(int max):
			m_lut(max)
		{}

		//����Ӧ�ķ��� �� ���� �󶨵�ʵ��
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

		//���˾��� ������� �������̬�󶨻ص��� ��ʯ��
		template<typename callback_T, typename ...arg_T>
		static typename std::result_of<callback_T(arg_T...)>::type invoke(void* target, int cid, arg_T&&... args)
		{
			//��ȡunique_ptr�����callback_object_base��
			auto dest = dynamic_cast<internal::callback_object<callback_T>*>(\
				reinterpret_cast<callbacks*>(target)->m_lut[cid].get());
			assert(dest);
			return dest->invoke(std::forward<arg_T>(args)...);
		}

	private:
		std::vector<callback_object_ptr> m_lut;
	};
}