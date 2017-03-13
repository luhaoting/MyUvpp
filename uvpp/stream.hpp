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
		bool listen(CallbackWithResult callback, int backlog = 128)//默认listen队列128
		{
			callbacks::store(Handle<handle_T>::get()->data, internal::eUVCallbackIdListen, callback);
			/*这个写法第一次看到, 继承类是模板类的时候 调用父类的模板方法
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
				源码里有这个代码

				// handle callback throwing exception: hold data in unique_ptr
				std::shared_ptr<char> baseHolder(buf->base, std::default_delete<char[]>());
				使用 shared_ptr 保护异常时的栈数据不丢失
				*/

				if (nread < 0)
				{
					/*
					http://blog.csdn.net/bywayboy/article/details/47137209
					对于网络断开事件 libuv没有设计专门的回调函数来进行处理,
					而是在 uv_read_start 所指定的回调函数中产生一个onread事件,
					通过 指定参数 nread < 0 来表示网络发生了断开事件。

					当连接断开时 服务器会根据断开原因，收到 UV_EOF 或者 UV_ECONNRESET 事件,
					而此时 uv_tcp 已经不能再用作写出数据。
					否则会产生一个异常。

					因此 正确的做法是 当收到 UV_EOF 的时候应当立即调用 uv_close() 来关闭连接。而非使用 uv_shutdown.
					*/

					//这是libuv cpp11 推荐Demo uvpp的代码
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
	};
}