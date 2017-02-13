#include <stdio.h>
#include <uv.h>
#include <malloc.h>
#include <iostream>
/*
#pragma comment(lib,"Ws2_32.lib") 
#pragma comment(lib,"Psapi.lib") 
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib,"Userenv.lib") 
上面的库需要手动加 日了狗了

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
	int&& rVal = getVal();	//右值引用
	int i = rVal;			//右值引用可以接住临时变量而不复制。
	int ci = i;
	g(ci);
	std::cout << ci << std::endl;
	std::cout << i << std::endl;

	uv_loop_t *loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
	uv_loop_init(loop);

	uv_timer_t timer;
	uv_timer_init(loop, &timer);					//初始化定时器
	uv_timer_start(&timer, PrintPoint, 10000, 0);	//设定10s定时器
	uv_timer_set_repeat(&timer, 1000);				//设定次定时器执行之后,每1s重复一次
	
	uv_run(loop, UV_RUN_DEFAULT);
	uv_loop_close(loop);

	free(loop);
	return 0;
}
