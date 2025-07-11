#include<stdio.h>
#include<stdlib.h>
#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>
int main()
{
	unsigned long x = 0;
	x = syscall(223);        //测试223号系统调用
	printf("syscall result: %ld\n", x);
	return 0;
}