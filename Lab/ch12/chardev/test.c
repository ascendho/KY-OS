#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
int main()
{
	char buf[4096] = {"I have already told you 1 time hello world"};
	int fd =open("/dev/hello",O_RDWR );
	int ret = read(fd ,buf ,sizeof(buf));
	buf[ret] = '\0';
	printf("%s\n",buf);
}