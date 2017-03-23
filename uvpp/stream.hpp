#pragma once

#include "error.hpp"
#include <algorithm>
#include <memory>

namespace uv {

	template<typename handle_T>
	class Stream : public Handle<handle_T>
	{
	protected:
		Stream() :
			Handle<handle_T>()
		{}

	public:
		bool listen(CallbackWithResult callback, int backlog = 128)//Ĭ��listen����128
		{
			callbacks::store(Handle<handle_T>::get()->data, internal::eUVCallbackIdListen, callback);
			/*���д����һ�ο���, �̳�����ģ�����ʱ�� ���ø����ģ�巽��
			*
			* BaseClass<class_T>::template m_Func<func_T>();
			*/
			return uv_listen(Handle<handle_T>::template get<uv_stream_t>(), backlog,
				[](uv_stream_t* s, int status)
			{
				callbacks::invoke<decltype(callback)>(s->data, uv::internal::eUVCallbackIdListen, Error(status));
			})
		}

		bool accept(Stream& client)
		{
			return uv_accept(Handle<T>::template get<uv_stream_t>(), client.Handle<T>::template get<uv_stream_t>()) == 0;
		}

		template <int max_alloc_size>
		bool read_start(std::function<void(const char* buff, ssize_t len)> callback)
		{
			callbacks::store(Handle<handle_T>::get()->data, uv::internal::eUVCallbackIdReadStart, callback);
			
			return uv_read_start(Handle<handle_T>::template get<uv_stream_t>(), 
				[](uv_handle_t*, size_t suggested_size, uv_buf_t* buf)/*uv_readstart_cb*/
			{
				assert(buf);
				auto size = std::max(suggested_size, max_alloc_size);
				buf->base = new char[size];
				buf->len = size;
			},
				[](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf)
			{
				/*
				Դ�������������

				// handle callback throwing exception: hold data in unique_ptr
				std::shared_ptr<char> baseHolder(buf->base, std::default_delete<char[]>());
				�²���ʹ�� shared_ptr �����쳣ʱ��ջ���ݲ���ʧ
				*/

				if (nread < 0)
				{
					/*
					http://blog.csdn.net/bywayboy/article/details/47137209
					��������Ͽ��¼� libuvû�����ר�ŵĻص����������д���,
					������ uv_read_start ��ָ���Ļص������в���һ��onread�¼�,
					ͨ�� ָ������ nread < 0 ����ʾ���緢���˶Ͽ��¼���

					�����ӶϿ�ʱ ����������ݶϿ�ԭ���յ� UV_EOF ���� UV_ECONNRESET �¼�,
					����ʱ uv_tcp �Ѿ�����������д�����ݡ�
					��������һ���쳣��

					��� ��ȷ�������� ���յ� UV_EOF ��ʱ��Ӧ���������� uv_close() ���ر����ӡ�����ʹ�� uv_shutdown.
					*/

					//����libuv cpp11 �Ƽ�Demo uvpp�Ĵ���
					//assert(nread == UV_EOF);
					//callbacks::invoke<decltype(callback)>(s->data, uvpp::internal::uv_cid_read_start, nullptr, nread);

					close();
				}
				else if (nread >= 0)
				{
					callbacks::invoke<decltype(callback)>(s->data, uv::internal::eUVCallbackIdReadStart, buf->base, nread);
				}) == 0;
			}
		}

		bool read_start(std::function<void(const char* buff, ssize_t len)> callback)
		{
			return read_start<0>(callback);
		}

		bool read_stop()
		{
			return uv_read_stop(Handle<handle_T>::template get<uv_stream_t>()) == 0;
		}

		//�����˶��write����
		bool write(const char* buf, ssize_t len, CallbackWithResult callback)
		{
			//�� buf ��ʼ�� uv_buf_t
			uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf), buf.length() } };

			callbacks::store(Handle<handle_T>::get()->data, uv::internal::eUVCallbackIdWrite, callback);
			
			return uv_write(new uv_write_t, Handle<handle_T>::template get<uv_stream_t>(), bufs, 1, 
				[](uv_write_t* req, int status)
			{
				std::unique_ptr<uv_write_t> reqHolder(req);//������unique_ptr �ͷ���req
				callbacks::invoke<decltype(callback)>(req->handle->data, uv::internal::eUVCallbackIdWrite, error(status));
			}) == 0;
		}

		bool write(const std::vector<char> buf, CallbackWithResult callback)
		{
			uv_buf_t bufs[] = { uv_buf_t {const_cast<char*>(&buf[0], buf.size()), } }	//vector ��������ǿת char* ��!!

			callbacks::store(Handle<handle_T>::get()->data, uv::internal::eUVCallbackIdWrite, callback);

			return uv_write(new uv_write_t, Handle<handle_T>::template get<uv_stream_t>(), bufs, 1,
				[](uv_write_t* req, int status)
			{
				std::unique_ptr<uv_write_t> reqHolder(req);//������unique_ptr �ͷ���req
				callbacks::invoke<decltype(callback)>(req->handle->data, uv::internal::eUVCallbackIdWrite, error(status));
			}) == 0;
		}

		bool write(const std::string& buf, CallbackWithResult callback)
		{
			uv_buf_t bufs[] = { uv_buf_t{ const_cast<char*>(buf.c_str(), buf.size()), } }

			callbacks::store(handle<HANDLE_T>::get()->data, uvpp::internal::uv_cid_write, callback);

			return uv_write(new uv_write_t, handle<HANDLE_T>::template get<uv_stream_t>(), bufs, 1, 
				[](uv_write_t* req, int status)
			{
				std::unique_ptr<uv_write_t> reqHolder(req);
				callbacks::invoke<decltype(callback)>(req->handle->data, uvpp::internal::uv_cid_write, error(status));
			}) == 0;
		}

		bool shutdown(CallbackWithResult callback)
		{
			callbacks::store(Handle<handle_T>::get()->data, uv::internal.eUVCallbackIdShutdown, callback);
			return uv_shutdown(new uv_shutdown_t, Handle<handle_T>::template get<uv_stream_t>(), 
				[](uv_shutdown_t* req, int status)
			{
				std::unique_ptr<uv_write_t> reqHolder(req);
				callbacks::invoke<decltype(callback)>(req->handle->data, internal::eUVCallbackIdShutdown, Error(status));
			}) == 0;
		}
	};
}