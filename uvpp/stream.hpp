#include "error.hpp"
#include <algorithm>
#include <memory>

namespace uv {

	template<typename handle_T>
	class Stream : public Handle<handle_T>
	{
	protected:
		Stream():
			Handle<handle_T>()
		{}

	public:
		bool listen(CallbackWithResult callback, int backlog = 128)//默认listen队列128
		{
			callbacks::store(Handle<handle_T>::get()->data, internal::eUVCallbackIdListen, callback);
			/*这个写法第一次看到, 继承类是模板类的时候 调用父类的模板方法
			* 从来没这么用过,所以不认识。。。
			* BaseClass<class_T>::template m_Func<func_T>();
			*/
			return uv_listen(Handle<handle_T>::template get<uv_stream_t>(), backlog, 
				[](uv_stream_t* s , int status)
			{
				callbacks::invoke<decltype(callback)>(s->data, uv::internal::eUVCallbackIdListen, Error(status));
			})
		}
		
	};
}