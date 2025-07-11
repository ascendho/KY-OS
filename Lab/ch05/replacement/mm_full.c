#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define TRUE 1
#define FALSE 0
#define INVALID -1
#define total_instruction 320
#define total_vp 32
#define clear_period 50
typedef struct   //页面结构
{
  int pn;	//页号
  int pfn;	//内存块号
  int counter;	//一个周期内访问页面的次数
  int time;	//访问时间
}p1_type;
p1_type p1[total_vp];
typedef struct pfc_struct  //页面控制结构
{
  int pn;	//页号
  int pfn;	//内存块号
  struct pfc_struct *next;
}pfc_type;
pfc_type pfc[total_vp];	//用户进程虚页控制结构
pfc_type *freepf_head;	//空内存页头指针
pfc_type *busypf_head;	//忙内存页头指针
pfc_type *busypf_tail;	//忙内存页尾指针
int diseffect,a[total_instruction];
int page[total_instruction],offset[total_instruction];
void initialize();
void FIFO();
void LRU();
void OPT();
int main()
{
  int s,i,j;
  srand (10*getpid());
  s=(float)319*rand()/32767/32767/2+1;
  for(i=0;i<total_instruction;i+=4)
  {
    if(s<0||s>319)
    {
      printf("When i==%d, Error, s==%d\n",i,s);
      exit(0);
    }
    a[i]=s;
    a[i+1]=a[i]+1;
    a[i+2]=(float)a[i]*rand()/32767/32767/2;
    a[i+3]=a[i+2]+1;
    s=(float)(318-a[i+2])*rand()/32767/32767/2+a[i+2]+2;
    if((a[i+2]>318)||(s>319))
      printf("a[%d+2],a number which is: %d and s==%d\n",i,a[i+2],s);
  }
  //指令序列变换为页面地址流
  for(i=0;i<total_instruction;i++)
  {
    page[i]=a[i]/10;
    offset[i]=a[i]%10;
  }
  //用户工作区从4个页面到32个页面
  for(i=4;i<=32;i++)
  {
    printf("%2d page frames",i);
    FIFO(i);
    LRU(i);
    OPT(i);
    printf("\n");
  }
}
void initialize(int total_pf)
{
  int i;
  diseffect=0;
  for(i=0;i<total_vp;i++)
  {
    p1[i].pn=i;
    p1[i].pfn=INVALID;
    p1[i].counter=0;
    p1[i].time=-1;
  }
  for(i=0;i<total_pf-1;i++)
  {
    pfc[i].next=&pfc[i+1];
    pfc[i].pfn=i;
  }
  pfc[total_pf-1].next=NULL;
  pfc[total_pf-1].pfn=total_pf-1;
  freepf_head=&pfc[0];
}
void FIFO(int total_pf)
{
  int i,j;
  pfc_type *p;
  initialize(total_pf);
  busypf_head=busypf_tail=NULL;
  for(i=0;i<total_instruction;i++)
  {
    if(p1[page[i]].pfn==INVALID)	//页面失效
    {   
      diseffect+=1;			//失效次数
      if(freepf_head==NULL)		//无空闲页面
      {
        p=busypf_head->next;
        p1[busypf_head->pn].pfn=INVALID;
        freepf_head=busypf_head;	//释放忙页面的第一个页面
        freepf_head->next=NULL;
        busypf_head=p;
      }
      p=freepf_head->next;	//按FIFO方式调入新页面到内存页面
      freepf_head->next=NULL;
      freepf_head->pn=page[i];
      p1[page[i]].pfn=freepf_head->pfn;
      if(busypf_tail==NULL)
        busypf_head=busypf_tail=freepf_head;
      else
      {
        busypf_tail->next=freepf_head; 	//空闲页面减少一个
        busypf_tail=freepf_head;
      }
      freepf_head=p;
    }
  }
  printf(" FIFO:%6.4f ",1-(float)diseffect/320);
}

void LRU(int total_pf)   /*LRU算法*/
{
  int min,minj,i,j,present_time;
  initialize(total_pf);
  present_time=0;

  for(i=0;i<total_instruction;i++)
  {
    if(p1[page[i]].pfn==INVALID)   /*页面失效*/
    {
      diseffect++;

      if(freepf_head==NULL)   /*无空闲页面*/
      {
        min=32767;
        for(j=0;j<total_vp;j++)   /*找出time的最小值*/
          if(min>p1[j].time && p1[j].pfn!=INVALID)
          {
            min=p1[j].time;
            minj=j;
          }
         freepf_head=&pfc[p1[minj].pfn]; /*腾出一个单元*/
         p1[minj].pfn=INVALID;
         freepf_head->next=NULL;
      }
      p1[page[i]].pfn=freepf_head->pfn;  /*有空闲页面，改为有效*/
      p1[page[i]].time=present_time;
 
      freepf_head=freepf_head->next;   /*减少一个空闲页面*/
    } 
    else
      p1[page[i]].time=present_time; /*命中，则增加该单元到访问次数*/
    present_time++;
  }
//printf("diseffect=%d\r\n",diseffect);
  printf("LRU:%6.4f ",1-(float)diseffect/320);
}

void OPT(int total_pf)
{
  int i,j,max,maxpage,d,dist[total_vp];
  pfc_type *t;
  initialize(total_pf);
  for(i=0;i<total_instruction;i++)
  {
    if(p1[page[i]].pfn==INVALID)   /*页面失效*/
    {
      diseffect++;
      if(freepf_head==NULL)   /*无空闲页面*/
      {
        for(j=0;j<total_vp;j++)
          if(p1[j].pfn!=INVALID)
            dist[j]=32767;
          else
            dist[j]=0;
        d=1;
        for(j=j+1;j<total_instruction;j++)
        {
          if(p1[page[j]].pfn!=INVALID)
            dist[page[j]]=d;
          d++;
        }
        max=1;
        for(j=0;j<total_vp;j++)
          if(max<dist[j])
          {
            max=dist[j];
            maxpage=j;
          }
        freepf_head=&pfc[p1[maxpage].pfn];
        freepf_head->next=NULL;
        p1[maxpage].pfn=INVALID;
      }
      p1[page[i]].pfn=freepf_head->pfn;
      freepf_head=freepf_head->next;
    }
  }
  printf("OPT:%6.4f ",1-(float)diseffect/320);
}


