#include <unistd.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include<time.h>

#define MAX_DATA_LEN   256  
#define DELAY_TIME 1  

/*
数据结构
0：石头
1：剪刀
2：布*/

/*judge函数返回值含义
1：甲赢
0：平局
-1：乙赢*/

int judge(char a,char b)   //胜负判定函数
{
    if(a==b)
        return 0;
    else
    {
        if(a=='0'&&b =='1')
            return 1;
        if(a=='0'&&b =='2')
            return -1;
        if(a=='1'&&b =='2')
            return 1;
        if(a=='1'&&b =='0')
            return -1;
        if(a=='2'&&b =='0')
            return 1;
        if(a=='2'&&b =='1')
            return -1;
    }
}


int main()  
{     
    /*创建第一个管道和第一个子进程*/
     pid_t pid;     
      
     int pipe_fd[2];    
     char buf[MAX_DATA_LEN];    
     char data[20];   
     int real_read, real_write;   //管道的读写操作
     
     memset((void*)buf, 0, sizeof(buf));   //清空缓冲区  
   
     if (pipe(pipe_fd) < 0)   //判断管道是否创建成功 
     {         
         printf("pipe create error\n");       
         exit(1);    
     }  
         
    if ((pid = fork()) == 0)     //创建第一个子进程
     {
     
         close(pipe_fd[0]);      // 子进程关闭读描述符      
         sleep(3);               //通过使子进程暂停3s等待父进程已关闭相应的写描述符  
                    
          //子进程在管道中写内容      
         srand(time(NULL));
         int i;
         for(i = 0;i<120;i++)
         {
             sprintf(data,"%d",rand()%3);//随机产生0-2的数字写入管道
             while(write(pipe_fd[1],data,strlen(data))==-1);//不断尝试写直到成功
         }   
  
         close(pipe_fd[1]);          
         exit(0);      
 }
 
   /*创建第二个管道和第二个子进程*/
     pid_t pid_1;    
       
     int pipe_fd_1[2];    
     char buf_1[MAX_DATA_LEN];    
     char data_1[20];   
     int real_read_1, real_write_1;
     
     memset((void*)buf, 0, sizeof(buf));   //清空缓冲区  
   
     if (pipe(pipe_fd_1) < 0)   //判断管道是否创建成功 
     {         
         printf("pipe create error\n");       
         exit(1);    
     }  
         
    if ((pid_1 = fork()) == 0)     //创建第一个子进程
     {
     
         close(pipe_fd_1[0]);      // 子进程关闭读描述符      
         sleep(3);               //通过使子进程暂停3s等待父进程已关闭相应的写描述符  
                    
          //子进程在管道中写内容      
         srand(time(NULL)+120);
         int i;
         for(i = 0;i<120;i++)
         {
             sprintf(data_1,"%d",rand()%3);//随机产生0-2的数字写入管道
             while(write(pipe_fd_1[1],data_1,strlen(data_1))==-1);//不断尝试写直到成功
         }   
  
         close(pipe_fd_1[1]);    //关闭子进程写描述符      
         exit(0);      
 }
 
   
         /*父进程*/
     else if (pid > 0)     
      {         
           /* 读取第一个管道的数据 */             
         close(pipe_fd[1]);      //父进程关闭写描述符      
         sleep(1);      //通过使父进程暂停1s等待子进程关闭相应的读描述符
               
         if((real_read = read(pipe_fd[0], buf, MAX_DATA_LEN)) > 0)     
           {          
             printf("读取选手甲数据成功！\n");  
           }     
                  
         close(pipe_fd[0]);     //关闭父进程读描述符   
         waitpid(pid, NULL, 0);  //收集子进程1退出信息
         
          /* 读取第二个管道的数据 */             
         close(pipe_fd_1[1]);      //父进程关闭写描述符      
         sleep(1);      //通过使父进程暂停1s等待子进程关闭相应的读描述符
               
         if((real_read_1 = read(pipe_fd_1[0], buf_1, MAX_DATA_LEN)) > 0)     
           {          
             printf("读取选手乙数据成功！\n\n");  
           }     
                  
         close(pipe_fd_1[0]);     //关闭父进程读描述符   
         waitpid(pid_1, NULL, 0);  //收集子进程2退出信息
           /*结束*/
         
           //判定比赛并公示结果
          int aw = 0,bw = 0,n_w = 0;
          int res = 0;
          int i;
          for(i = 0;i<120;i++){
              printf("第%d回合：",i+1);
              res = judge(buf[i],buf_1[i]);

              if(res == 1){
                  printf("甲赢！\n");
                  aw++;
                  }
              else if(res == -1){
                  printf("乙赢！\n");
                  bw++;
                  }
              else{
                  printf("平局！\n");
                  n_w++;
                  }
             }
             
          printf("\n比赛结果公示\n");
          printf("选手甲赢了：%d次\n",aw);
          printf("选手乙赢了：%d次\n",bw);
          printf("平局：%d次\n",n_w);
          exit(0);
       }   
          
             
     return 0;   
}
