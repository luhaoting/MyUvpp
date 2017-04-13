#pragma once

#include "stream.hpp"
#include "net.hpp"
#include "loop.hpp"

namespace uv {
    class Tcp : public Stream<uv_tcp_t>
    {
    public:
        Tcp() :
            Stream()
        {
            uv_tcp_init(uv_default_loop(), get());
        }

        Tcp(Loop& loop) :
            Stream()
        {
            uv_tcp_init(loop.get(), get());
        }

        bool nodelay(bool enable)
        {
            return uv_tcp_nodelay(get(), enable ? 1 : 0) == 0;
        }

        bool keep_alive(bool enable, unsigned int delay)
        {
            return uv_tcp_keepalive(get(), enable ? 1 : 0, delay) == 0;
        }

        bool simultanious_accept(bool enable)
        {
            return uv_tcp_simultaneous_accepts(get(), enable ? 1 : 0) == 0;
        }

        bool bind(const std::string& ip, const int port)
        {
            ip4_addr addr = to_ip4_addr(ip, port);
            return uv_tcp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0) == 0;
        }

        bool bind6(const std::string& ip, const int port)
        {
            ip6_addr addr = to_ip6_addr(ip, port);
            return uv_tcp_bind(get(), reinterpret_cast<sockaddr*>(&addr), 0) == 0;
        }

        bool connect(const std::string& ip, const int port, CallbackWithResult callback)
        {
            callbacks::store(get()->data, internal::eUVCallbackIdConnect, callback);
            ip4_addr ip4 = to_ip4_addr(ip, port);
            return uv_tcp_connect(new uv_connect_t(), get(), reinterpret_cast<sockaddr*>(&ip4),
                [](uv_connect_t* req, int status)
            {
                std::unique_ptr<uv_connect_t> reqHolder(req);
                callbacks::invoke<decltype(callback)>(req->handle->data, internal::eUVCallbackIdConnect, Error(status));
            }) == 0;
        }

        bool connect6(const std::string& ip, const int port, CallbackWithResult callback)
        {
            callbacks::store(get()->data, internal::eUVCallbackIdConnect6, callback);
            ip6_addr ip6 = to_ip6_addr(ip, port);
            return uv_tcp_connect(new uv_connect_t(), get(), reinterpret_cast<sockaddr*>(&ip6),
                [](uv_connect_t* req, int status)
            {
                std::unique_ptr<uv_connect_t> reqHolder(req);
                callbacks::invoke<decltype(callback)>(req->handle->data, internal::eUVCallbackIdConnect6, Error(status));
            }) == 0;
        }

        bool getsockname(bool& isIp4, std::string& ip, int& port)
        {
            struct sockaddr_storage addr;
            int len = sizeof(addr);
            if (uv_tcp_getsockname(get(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
            {
                isIp4 = (addr.ss_family == AF_INET);
                if (isIp4)
                {
                    return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
                }
                else
                {
                    return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
                }
            }
            return false;
        }

        bool getpeername(bool& ip4, std::string& ip, int& port)
        {
            struct sockaddr_storage addr;
            int len = sizeof(addr);
            if (uv_tcp_getpeername(get(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
            {
                ip4 = (addr.ss_family == AF_INET);
                if (ip4)
                {
                    return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
                }
                else
                {
                    return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
                }
            }
            return false;
        }
    };
}
