#pragma once

#include <assert.h>
#include <stdexcept>
#include <string>
#include <uv.h>

namespace uv
{
	class Exception : public std::runtime_error
	{
	public:
		Exception(const std::string & msg):
			std::runtime_error(msg)
		{}
	};

	class Error
	{
	public:
		Error(int c):
			m_error(c)
		{}
	public:
		explicit operator bool() const
		{
			return m_error != 0;
		}

		const char* str() const
		{
			return uv_strerror(m_error);
		}
	private:
		int m_error;
	};
}

