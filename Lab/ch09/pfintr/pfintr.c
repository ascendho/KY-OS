#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FILENAME "/proc/vmstat"
#define DEFAULTTIME 5
static void sig_handler(int signo);
int get_page_fault(void);
int readfile(char *data);
int exit_flag=0;
int page_fault;
int main(int argc,char **argv)
{
	struct itimerval v;
	int cacl_time;
	if(signal(SIGALRM,sig_handler) == SIG_ERR){
			printf("Unable to create handler for SIGALRM\n");
			return -1;
		}	
	if(argc <= 2)
		page_fault = get_page_fault();
	/*初始化 timer_real*/
	if(argc < 2){
		printf("Use default time!\n");
		cacl_time = DEFAULTTIME;
	}
	else if(argc == 2){
		printf("Use user's time\n");
		cacl_time = atoi(argv[1]);
	}
	else if(argc > 2){
		printf("Usage:mypage [time]\n");
		return 0;
	}
	v.it_interval.tv_sec = cacl_time; 	/*设置产生信号的间隔时间，秒*/  
	v.it_interval.tv_usec = 0;			/*设置产生信号的间隔时间，微秒*/
	v.it_value.tv_sec = cacl_time;		/*设置第一次产生信号的时间，秒*/
	v.it_value.tv_usec = 0;			/*设置第一次产生信号的时间，微秒*/
	setitimer(ITIMER_REAL,&v,NULL);
	while(!exit_flag) ;
	printf("In %d seconds,system calls %d page fault!\n",cacl_time,page_fault);
	return 0;
}
static void sig_handler(int signo)
{
	if(signo == SIGALRM)
	/*当 ITIMER_REAL 为 0 时,这个信号被发出*/
	{
		page_fault = get_page_fault()-page_fault;
		exit_flag = 1;
	}
}
/*该函数通过调用文件操作函数 readfile,得到当前系统的缺页中断次数*/
int get_page_fault(void){
	char d[50];
	int retval;
/*读取缺页中断次数*/
	retval = readfile(d);
	if(retval<0){
		printf("read data from file failed!\n");
		exit(0);
	}
	printf("Now the number of page fault is %s\n",d);
	return atoi(d);
}
/*该函数对/proc/stat 文件内容进行读操作,读取指定项的值*/
int readfile(char *data){
	int fd;
	int seekcount = 0;
	int retval = 0;
	int i = 0;
	int count = 0;
	char c,string[50];
	fd = open(FILENAME,O_RDONLY);
	if(fd < 0)
	{
		printf("Open file /proc/stat failed!\n");
		return -1;
	}
/*查找 vmstat 文件中的关键字 intr */
	do{
		i=0;
		do{
			lseek(fd,seekcount,SEEK_SET);
			retval = read(fd,&c,sizeof(char));
			if(retval < 0)
				{
					printf("read file error!\n");
					return retval;
				}
			seekcount += sizeof(char);
			if(c == ' ' || c == '\n')
				{
					string[i] = 0;
					break;
				}
			if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c>= 'A' && c <='Z'))
				string[i++] = c;
		}while(1);
	}while(strcmp("pgfault",string));
	printf("find intr!\n");
/*读取缺页次数*/
	i=0;
	do{
		lseek(fd,seekcount,SEEK_SET);
		retval = read(fd,&c,sizeof(char));
		if(retval < 0)
		{
			printf("read file error!\n");
			return retval;
		}
		seekcount += sizeof(char);
		if(c == ' ' || c == '\n')
		{
			string[i] = 0;
			i = 0;
			count++;
		}
		if((c >= '0' && c <= '9') || (c >= 'a' && c<= 'z') || (c >= 'A' && c<= 'Z'))
		string[i++] = c;
	}while(count != 1);
	close(fd);
	strcpy(data,string);
	return 0;
}