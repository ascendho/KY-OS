#include "stdio.h" 
#include <stdlib.h> 
#define getpch(type) (type*)malloc(sizeof(type)) 

struct pcb { /* 定义进程控制块PCB */ 
  char name[10]; //进程名
  char state;    //进程状态："W"-就绪态，"R"-运行态
  int nice;      //进程优先级
  int ntime;     //需要运行时间
  int rtime;     //已经运行的时间
  struct pcb* link; 
}*ready=NULL,*p; 
typedef struct pcb PCB; 

char sort() /* 建立对进程进行优先级排列函数，优先数大者优先*/ 
{ 
  PCB *first, *second; 
  int insert=0; 
  if((ready==NULL)||((p->nice)>(ready->nice)))/*优先级最大者,插入队首*/ 
  { 
    p->link=ready; 
    ready=p; 
  } 
  else /* 进程比较优先级,插入适当的位置中*/ 
  { 
    first=ready; 
    second=first->link; 
    while(second!=NULL) 
    { 
      if((p->nice)>(second->nice)) /*若插入进程比当前进程优先数大,*/ 
      { /*插入到当前进程前面*/ 
        p->link=second; 
        first->link=p; 
        second=NULL; 
        insert=1; 
      } 
      else /* 插入进程优先数最低,则插入到队尾*/ 
      { 
        first=first->link; 
        second=second->link; 
      } 
    } 
    if(insert==0) first->link=p; 
  } 
} 

char input() /* 建立进程控制块函数*/ 
{ 
  int i,num; 
  printf("\n 请输入被调度的进程数目："); 
  scanf("%d",&num); 
  for(i=0;i<num;i++) 
  { 
    printf("\n 进程号No.%d:",i); 
    p=getpch(PCB); 
    printf("\n 输入进程名:"); 
    scanf("%s",p->name); 
    printf(" 输入进程优先数:"); 
    scanf("%d",&p->nice); 
    printf(" 输入进程运行时间:"); 
    scanf("%d",&p->ntime); 
    printf("\n"); 
    p->rtime=0;
    p->state='W'; 
    p->link=NULL; 
    sort(); /* 调用sort函数*/ 
  } 
} 

int space() 
{ 
  int l=0; PCB* pr=ready; 
  while(pr!=NULL) 
  { 
    l++; 
    pr=pr->link; 
  } 
  return(l); 
} 

char disp(PCB * pr) /*建立进程显示函数,用于显示当前进程*/ 
{ 
  printf("\n qname \t state \t nice \tndtime\truntime \n"); 
  printf("%s\t",pr->name); 
  printf("%c\t",pr->state); 
  printf("%d\t",pr->nice); 
  printf("%d\t",pr->ntime); 
  printf("%d\t",pr->rtime); 
  printf("\n"); 
}

char check() /* 建立进程查看函数 */ 
{ 
  PCB* pr; 
  printf("\n **** 当前正在运行的进程是:%s",p->name); /*显示当前运行进程*/ 
  disp(p); 
  pr=ready; 
  if (pr!=NULL) 
    printf("\n ****当前就绪队列状态为:"); /*显示就绪队列状态*/
  else 
    printf("\n ****当前就绪队列状态为: 空\n"); /*显示就绪队列状态为空*/
  while(pr!=NULL) 
  { 
    disp(pr); 
    pr=pr->link; 
  } 
} 

char destroy() /*建立进程撤消函数(进程运行结束,撤消进程)*/ 
{ 
  printf(" 进程 [%s] 已完成.\n",p->name); 
  free(p); 
}
 
char running() /* 建立进程就绪函数(进程运行时间到,置就绪状态*/ 
{ 
  (p->rtime)++; 
  if(p->rtime==p->ntime) 
  destroy(); /* 调用destroy函数*/ 
  else 
  { 
    (p->nice)--; 
    p->state='W'; 
    sort(); /*调用sort函数*/ 
  } 
} 

main() /*主函数*/ 
{ 
  int len,h=0; 
  char ch; 
  input(); 
  len=space(); 
  while((len!=0)&&(ready!=NULL)) 
  { 
    ch=getchar(); 
    h++; 
    printf("\n The execute number:%d \n",h); 
    p=ready; 
    ready=p->link; 
    p->link=NULL; 
    p->state='R'; 
    check(); 
    running(); 
    printf("\n按任一键继续......"); 
    ch=getchar(); 
  } 
  printf("\n\n 所有进程已经运行完成！\n"); 
  ch=getchar(); 
}

