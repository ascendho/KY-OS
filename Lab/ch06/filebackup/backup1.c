#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
int main()
{
	char buf;
	FILE *source,*backup;
        printf("This program  backup file based on C Library.\n");
	source=fopen("./source.dat","r");
	backup=fopen("./target.dat","w");
	if(source==NULL)
	{
		printf("Error in opening file.\n");
		exit(1);
	}
	if(backup==NULL)
	{
		printf("Error in creating backup file.\n");
		exit(1);
	}
	while(fread(&buf,sizeof(buf),1,source)==1)
	{
		if(!fwrite(&buf,sizeof(buf),1,backup))
		{
			printf("Error in writing file.\n");
			exit(1);
		}
	}
	if(ferror(source)!=0)
	{
		printf("Error in reading source file.\n");
		exit(1);
	}
	else 
		printf("Success in reading source file.\n");
	if(fclose(source))
	{
		printf("Error in close source file.\n");
		exit(1);
	}
	else 
		printf("Success in close source file.\n");
	if(fclose(backup))
	{
		printf("Error in close target file.\n");
		exit(1);
	}
	else 
		printf("Success in close target file.\n");
        exit(0);
}
