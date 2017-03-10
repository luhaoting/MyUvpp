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
		bool listen(CallbackWithResult callback, int backlog = 128)//Ĭ��listen����128
		{
			callbacks::store(Handle<handle_T>::get()->data, internal::eUVCallbackIdListen, callback);
			/*���д����һ�ο���, �̳�����ģ�����ʱ�� ���ø����ģ�巽��
			* ����û��ô�ù�,���Բ���ʶ������
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