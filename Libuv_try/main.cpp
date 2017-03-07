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
上面的库需要手动加 日了狗了

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
	//int&& rVal = getVal();	//右值引用
	//int i = rVal;			//右值引用可以接住临时变量而不复制。
	//int ci = i;
	//g(ci);
	//std::cout << ci << std::endl;
	//std::cout << i << std::endl;

	uv_loop_t *loop = uv_default_loop();

	uv_timer_t *timer = new uv_timer_t();
	uv_timer_init(loop, timer);					//初始化定时器
	uv_timer_start(timer, PrintPoint, 10000, /*1000*/0);	//设定10s定时器,每1s重复一次
	//uv_timer_set_repeat(&timer, 1000);				//设定次定时器执行之后,每1s重复一次
	
	uv_run(loop, UV_RUN_DEFAULT);
	free(timer);
	uv_loop_close(loop);
	free(loop);

	//uv::Timer timer;

	//uv::Loop loop;
	//timer.Init(loop);
	//std::chrono::duration<int, std::milli> time(1000);
	//timer.start([]() {std::cout << " ." << std::endl; }, time);
	//loop.run();
	return 0;
}
