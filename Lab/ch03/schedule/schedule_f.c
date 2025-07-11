#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

typedef struct node{
  char name[10];
  int prio;
  int round;
  int cputime;
  int needtime;
  int count;
  char state;
  struct node *next;
}PCB;

PCB *finish,*ready,*run,*r;
int N;

void firstin()
{
  run = ready;
  run->state = 'R';
  ready = ready->next;
}
   
void prt1(char a)
{
  if(a=='P'||a == 'p') 
    printf("进程号  cpu时间   所需时间  优先数   状态\n");
  else if(a =='r'||a =='R')
    printf("进程号  cpu时间   所需时间   记数   时间片   状态\n");
  else if(a=='f'||a=='F')
    printf("进程号   所需时间   状态\n");
}

void prt2(char a ,PCB *q)
{
     if(a=='P'||a == 'p')
     printf("%-10s%-10d%-10d%-10d%c\n",q->name,q->cputime,q->needtime,q->prio,q->state);
     else if(a == 'r'|| a == 'R')
     printf("%-10s%-10d%-10d%-10d%-10d%c\n",q->name,q->cputime,q->needtime,q->count,q->round,q->state);
     else if(a=='f'||a=='F')
     printf("%-10s%-10d%c\n",q->name,q->needtime,q->state);
}

void prt(char algo)
{
     PCB *p;
     prt1(algo);
     if(run != NULL&& !(algo == 'r'||algo  =='R'))//这个对RR不适应！！ 
         prt2(algo,run);
     p = ready;
     while(p != NULL)
     {
         prt2(algo,p);
         p = p->next;
     }
     p = finish;
     while(p != NULL)
     {
         prt2(algo,p);
         p = p->next;
     }
     getchar();
}

void insert1(PCB *q)
{
   PCB *p1,*s,*r;
   int b;
   s = q;
   p1 = ready;
   r = p1;
   b = 1;
   while((p1 != NULL)&&b)
    {
       if(p1->prio >= s->prio)
       {
         r = p1;
         p1 = p1->next;
       }
       else b = 0;
    }
    if(r!= p1)
    {
      r->next = s;
      s->next = p1;
    }
    else
    {
      s->next = p1;
      ready = s;
    }
}

void creat1(char alg)
{
     PCB *p;
     int i,time;
     char na[10];
     ready = NULL;
     finish = NULL;
     run = NULL;
     printf("输入进程号和运行时间:\n");
     for(i = 1; i <= N; i++)
     {
       p = (PCB *)malloc(sizeof(PCB));
       scanf("%s",na);
       scanf("%d",&time);
       strcpy(p->name,na);
       p->cputime = 0;
       p->needtime = time;
       p->state = 'W';
       p->prio = 50-time;
       if(ready != NULL)
       insert1(p);
       else{
         p->next = ready;
         ready = p;
         }
     }
     //clrscr();
     if(alg =='p'||alg =='P')
       printf("   优先数算法输出信息:\n");
     else
       printf("   先来先服务算法输出信息:\n");
     printf("*****************************************\n");
     prt(alg);
     run = ready;
     ready = ready->next;
     run->state = 'R';
}

void creat2(char alg)
{
     PCB *p;
     int i,time,round;
     char na[10];
     ready = NULL;
     finish = NULL;
     run = NULL;
     printf("请输入时间片：");
     scanf("%d",&round);
     printf("输入进程号和运行时间:\n");
     for(i = 1; i <= N; i++)
     {
       p = (PCB *)malloc(sizeof(PCB));
       scanf("%s",na);
       scanf("%d",&time);
       strcpy(p->name,na);
       p->cputime = 0;
       p->needtime = time;
       p->state = 'W';
       p->count  = 0;
       p->round  = round;
       p->next = NULL;
        if(i == 1)/*按顺序插入到ready链表中*/
       r = ready = p;
       else
       r->next = p;
       r = p;  
     }
     //clrscr();
     printf("     时间片轮转算法输出信息:\n");
     printf("*****************************************\n");
     prt(alg);
}

void priority(char alg)  //优先数调度算法
{
     while(run != NULL)
     {
       run->cputime += 1;
       run->needtime -= 1;
       run->prio -= 3;
       if(run->needtime == 0)
        {
          run->next = finish;
          finish = run;
          run->state = 'F';
          run = NULL;
          if(ready != NULL)
          firstin();
        }
        else 
         if((ready != NULL) && (run->prio < ready->prio))
         {
            run->state = 'W';
            insert1(run);
            firstin();
         }
         prt(alg);
     }
}

void roundrun(char alg)  //时间片轮转调度算法
{
  bool flag;//当ready列里只有一个时做标记 
  while(N){
    flag = 1;//初始化为1 
    run = ready;//run每次运行ready的队头 
    run->count++;//每运行一次计数器加1 
    if(run->needtime < run->round)//当剩余时间小于时间片轮转时间时的情况 
            {
              run->cputime += run->needtime;
              run->needtime = 0;
            }
            else{
              run->cputime += run->round;
              run->needtime -= run->round;  
                }
         run->state = 'W';//变为等待 
        if(ready->next != NULL)
        ready = ready->next;
        else  flag = 0;//当ready剩一个时做标记 
        if(run->needtime == 0){//当run结束时放入finish队列里 
          run->next = finish;
          finish = run;
          run->state = 'F';
          N--;//进程数少1 
          }
          else{
          if(flag){//执行完如果不是剩一个的话，就把run放到队尾 
           r->next = run;
           r = run;
           r->next = NULL; 
                 }
              }
           if(N)ready->state = 'R';//结束时不应该有'R" 
           else
           ready = NULL;//结束时应该为空 
           prt(alg);//输出 
    }
}

void FCFSrun(char alg)  //先来先服务调度算法
{  
  PCB *p;
  while(run!=NULL)
  {
    run->cputime += run->needtime;
    run->needtime =0;               
    run->next = finish;
    finish = run;
    run->state = 'F';
    run = NULL;
    if(ready!=NULL)
      firstin();
      prt(alg);
    }
  }

main()
{
      char algo;
      //clrscr();
      printf("选择算法:P/R/F(优先数算法/时间片轮转算法/先来先服务算法)\n");
      scanf("%c",&algo);
      printf("输入进程数:\n");
      scanf("%d",&N);
      if(algo == 'P'||algo == 'p')
      {
        creat1(algo);
        priority(algo);
      }
      else if(algo == 'R'||algo == 'r')
      {
        creat2(algo);
        roundrun(algo);
      }
      else if(algo=='f'||algo=='F')
      {
         creat1(algo);
         FCFSrun(algo);     
      }
     system("echo 按任意键退出");
     system("read -n 1");
     return 0;
} 

