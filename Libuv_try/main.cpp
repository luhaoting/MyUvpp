#include <stdio.h>
#include <uv.h>
#include <malloc.h>
#include <iostream>
#include <thread>

/*
#pragma comment(lib,"Ws2_32.lib") 
#pragma comment(lib,"Psapi.lib") 
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"Userenv.lib") 
����Ŀ���Ҫ�ֶ��� ���˹���

#pragma comment(lib,"libuv.lib") 
*/


#include "loop.hpp"
#include "timer.hpp"

static void PrintPoint(uv_timer_t* t)
{
	std::cout << " ." ;
}

static void PrintPoint1()
{
	std::cout << " .";
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
	//int&& rVal = getVal();	//��ֵ����
	//int i = rVal;			//��ֵ���ÿ��Խ�ס��ʱ�����������ơ�
	//int ci = i;
	//g(ci);
	//std::cout << ci << std::endl;
	//std::cout << i << std::endl;

	//uv_loop_t *loop = uv_default_loop();
	//uv_timer_t *timer = new uv_timer_t();
	//uv_timer_init(loop, timer);							//��ʼ����ʱ��
	//uv_timer_start(timer, PrintPoint, 10000, /*1000*/0);	//�趨10s��ʱ��,ÿ1s�ظ�һ��
	////uv_timer_set_repeat(&timer, 1000);					//�趨�ζ�ʱ��ִ��֮��,ÿ1s�ظ�һ��
	//
	//uv_run(loop, UV_RUN_DEFAULT);
	//free(timer);  
	//uv_loop_close(loop);
	//free(loop);
	//timer 


	//  int uv_loop_close(uv_loop_t* loop)
    //  �ͷ����е�ѭ���ڲ�����Դ��ֻ����loop������в������д򿪵�handle�Լ�requests�Ѿ����رգ�
	//	����᷵��UV_EBUSY��������������֮���û������ͷ�Ϊloop������ڴ���

	//���ֹ��췽ʽ ����uv-common.c line��635 ������ԭ��handle��loop֮ǰ��free loop�����ٴ������� 
	//uv::Loop loop;
	//uv::Timer timer;
	//����handle::close();�Ͳ�����
	

	uv::Loop loop;
	uv::Timer timer; 

	timer.Init(loop);
	std::chrono::duration<int, std::milli> time(1000);
	timer.start([]() {std::cout << " ." << std::endl; }, time);
	loop.run();

	timer.close();
	return 0;
}
