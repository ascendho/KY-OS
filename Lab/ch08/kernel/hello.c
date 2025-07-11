#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
	long int a = syscall(333);
	printf("System call sys_helloworld reutrn %ld\n", a);
	return 0;
}