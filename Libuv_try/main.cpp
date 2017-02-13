#include <stdio.h>
#include <uv.h>
#include <malloc.h>
#include <iostream>
/*
#pragma comment(lib,"Ws2_32.lib") 
#pragma comment(lib,"Psapi.lib") 
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"Userenv.lib") 
����Ŀ���Ҫ�ֶ��� ���˹���

#pragma comment(lib,"libuv.lib") 
*/

static void PrintPoint(uv_timer_t *handle)
{
	
	std::cout << " ." ;
}

int getVal()
{
	int o = 2333;
	return o;
}

template <typename T>
void g(T& val)
{
	std::cout << val << std::endl;
	val = 99;
}

int main() 
{
	int&& rVal = getVal();	//��ֵ����
	int i = rVal;			//��ֵ���ÿ��Խ�ס��ʱ�����������ơ�
	int ci = i;
	g(ci);
	std::cout << ci << std::endl;
	std::cout << i << std::endl;

	uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
	uv_loop_init(loop);

	uv_timer_t timer;
	uv_timer_init(loop, &timer);					//��ʼ����ʱ��
	uv_timer_start(&timer, PrintPoint, 10000, 0);	//�趨10s��ʱ��
	uv_timer_set_repeat(&timer, 1000);				//�趨�ζ�ʱ��ִ��֮��,ÿ1s�ظ�һ��
	
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);

	free(loop);
	return 0;
}
