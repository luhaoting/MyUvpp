#include "EchoSvrConn.h"

void EchoSvrConn::OnRecv(const char *buff, ssize_t len)
{
   if (len > 0)
        {
            //����Ӧ�н��
            std::string str;
            str.append(buff, len);

            //free(buff);
            std::cout << str << std::endl;
            return;
        }
        else
        {
            m_handle.close(mf_close_cb);
        }
}