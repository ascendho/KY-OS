#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#define MAXSIZE 1024
int main()
{
	char buf[MAXSIZE];
	int source,backup;
        int size;
        printf("This program backup file based on Linux system calls.\n");
        //open file
	source=open("./source.dat",O_RDONLY);
	backup=open("./target.dat",O_WRONLY|O_CREAT,0644);
	if(source==-1)
	{
		perror("Error in opening file.\n");
		exit(1);
	}
	if(backup==-1)
	{
		perror("Error in opening backup file.\n");
		exit(1);
	}
        //backup file
	while((size=read(source,buf,MAXSIZE))>0)
	{
		if(write(backup,buf,size)!=size)
		{
			printf("Error in writing file.\n");
			exit(1);
		}
	}
	if(size<0)
	{
		printf("Error in reading source file.\n");
		exit(1);
	}
	else 
		printf("Success in reading source file.\n");
	//close files.
        if(close(source)<0)
	{
		perror("Error in close sourc file.");
		exit(1);
	}
	else 
		printf("Success in close source file.\n");
	if(close(backup)<0)
	{
		perror("Error in close target file.");
		exit(1);
	}
	else 
		printf("Success in close target file.\n");
        
        exit(0);
}
