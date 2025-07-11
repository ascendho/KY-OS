#include <stdio.h>   
#include <stdlib.h>   
#include <time.h>   
#include <string.h>
#include <unistd.h> 
#include <stdbool.h> 
#define MaxUser 100          //定义最大MDF主目录文件   
#define MaxDisk 512*1024     //模拟最大磁盘空间   
#define commandAmount 12     //对文件操作的指令数   
//存储空间管理有关结构体和变量   
char disk[MaxDisk];     //模拟512K的磁盘存储空间   
typedef struct distTable  //磁盘块结构体   
{   
    int maxlength;   
    int start;   
    int useFlag;   
    struct distTable *next;   
}diskNode;   
diskNode *diskHead;   
typedef struct fileTable    //文件块结构体，即FCB   
{   
    char fileName[10];   
    int strat;          //文件在磁盘存储空间的起始地址   
    int length;         //文件内容长度   
    int maxlength;      //文件的最大长度   
    char fileKind[3];   //文件的属性——读写方式   
    struct tm *timeinfo;   
    bool openFlag;       //判断是否有进程打开了该文件   
    //fileTable *next;   
}fileTableN;   
  
//两级目录结构体   
typedef struct user_file_directory  //用户文件目录文件UFD   
{   
    //char fileName[10];   
    struct fileTable *file;   
    struct user_file_directory *next;   
}UFD;   
//UFD *headFile;   
typedef struct master_file_directory  //主文件目录MFD   
{   
    char userName[10];   
    char password[10];   
    UFD *user;   
}MFD;   
MFD userTable[MaxUser];   
int used=0;                             //定义MFD目录中用已有的用户数   
  
//文件管理   
void fileCreate(char fileName[],int length,char fileKind[]);        //创建文件   
void fileWrite(char fileName[]);                    //写文件   
void fileCat(char fileName[]);                    //读文件   
void fileRen(char fileName[],char rename[]);        //重命名文件   
void fileFine(char fileName[]);                   //查询文件   
void fileDir(char UserName[]);                    //显示某一用户的所有文件   
void fileClose(char fileName[]);                  //关闭已打开的文件   
void fileDel(char fileName[]);                    //删除文件   
void chmod(char fileName[],char kind[]);         //修改文件的读写方式   
int requestDist(int startPostion,int maxLength); //磁盘分配查询   
void initDisk();                                  //初始化磁盘   
void freeDisk(int startPostion);                  //磁盘空间释放   
void diskShow();                                  //显示磁盘使用情况   
  
//用户管理   
void userCreate();   
int login();   
int userID=-1;   //用户登录的ID号，值为-1时表示没有用户登录   
  
int main()   
{   
    char order[commandAmount][10];   
    strcpy(order[0],"create");   
    strcpy(order[1],"rm");   
    strcpy(order[2],"cat");   
    strcpy(order[3],"write");   
    strcpy(order[4],"fine");   
    strcpy(order[5],"chmod");   
    strcpy(order[6],"ren");   
    strcpy(order[7],"dir");   
    strcpy(order[8],"close");   
    strcpy(order[9],"return");   
    strcpy(order[10],"exit");   
    strcpy(order[11],"df");   
    char command[100],command_str1[10],command_str2[10],command_str3[5],command_str4[3];   
    int i,k,j;   
    int length;   
    initDisk();                     //初始化磁盘   
    for(i=0;i<MaxUser;i++)          //初始化用户UFD目录文件的头指针   
    {   
        userTable[i].user=(UFD *)malloc(sizeof(UFD));   
        userTable[i].user->next=NULL;   
    }   
    while(1)   
    {   
        printf("********************************************\n");   
        printf("                  1、Creat user\n");   
        printf("                  2、login\n");   
        printf("********************************************\n");   
        printf("Please chooce the function key:>");   
        int choice;   
        scanf("%d",&choice); 
        if(choice==1) userCreate();   
        else if(choice==2) userID=login();   
        else printf("您的输入有误，请重新选择\n");   
        while(userID!=-1)   
        {               
            printf("———————————————————————————————————————\n");   
            printf(" create-创建 格式：create a1 1000 rw,将创建名为a1,长度为1000字节可读可写的文件\n");   
            printf(" rm-删除 格式：rm a1,将删除名为a1的文件\n");   
            printf(" cat-查看文件内容 格式：cat a1,显示a1的内容\n");   
            printf(" write-写入  格式：write a1\n");   
            printf(" fine-查询 格式：fine a1 ,将显示文件 a1的属性\n");   
            printf(" chmod-修改 格式：chmod a1 r,将文件a1的权限改为只读方式\n");   
            printf(" ren-重命名 格式：ren a1 b1 ,将a1改名为b1\n");   
            printf(" dir-显示文件 格式：dir aaa,将显示aaa用户的所有文件\n");   
            printf(" df-显示磁盘空间使用情况 格式：df\n");   
            printf(" close-关闭文件 格式：close a1,将关闭文件a1\n");   
            printf(" return-退出用户，返回登录界面\n");   
            printf(" exit-退出程序\n");   
            printf("————————————————————————————————————————\n");   
            printf("please imput your command:>");   
            gets(command);  
            int select;
            //command_str1字符串存储命令的操作类型   
            for(i=0;command[i]!=' '&&command[i]!='\0';i++)     
                command_str1[i]=command[i];   
            k=i;   
            command_str1[k]='\0';  
            for(i=0;i<commandAmount;i++)   
            {   
                if(!strcmp(command_str1,order[i]))   
                {   
                    select=i;   
                    break;   
                }   
            }   
            if(i==commandAmount)    
            {   
                printf("您输入的命令有误，请重新输入\n");   
                continue;   
            }   
            //commmand_str2字符串存储文件名或用户名
            for(i=k+1,k=0;command[i]!=' '&&command[i]!='\0';i++,k++)       
                command_str2[k]=command[i];   
            command_str2[k]='\0';   
            k=i;   
	    UFD *p; 
            switch(select)   
            {   
            case 0:for(i=k+1,k=0;command[i]!=' ';i++,k++)   
                       command_str3[k]=command[i];   
                command_str3[k]='\0';   
                k=i;   
                j=1;   
                length=0;                                  //初始化文件长度   
                for(i=strlen(command_str3)-1;i>=0;i--)     //把字符串转换为十进制   
                {   
                    length+=(command_str3[i]-48)*j;   
                    j*=10;   
                }   
                for(i=k+1,k=0;command[i]!=' '&&command[i]!='\0';i++,k++)   
                    command_str4[k]=command[i];   
                command_str4[k]='\0';   
                fileCreate(command_str2,length,command_str4);break;   
            case 1:fileDel(command_str2);break;   
            case 2:fileCat(command_str2);break;   
            case 3:   
                fileWrite(command_str2);break;   
            case 4:fileFine(command_str2);break;   
            case 5:for(i=k+1,k=0;command[i]!=' '&&command[i]!='\0';i++,k++)   
                       command_str3[k]=command[i];   
                command_str3[k]='\0';   
                chmod(command_str2,command_str3);break;   
            case 6:for(i=k+1,k=0;command[i]!='\0';i++,k++)   
                       command_str3[k]=command[i];   
                command_str3[k]='\0';   
                fileRen(command_str2,command_str3);break;   
            case 7:fileDir(command_str2);break;   
            case 8:fileClose(command_str2);break;   
            case 9: 	//退出用户之前关闭所有打的文件 
                for(p=userTable[userID].user->next;p!=NULL;p=p->next)    
                    if(p->file->openFlag) p->file->openFlag=false;   
                system("cls");   
                userID=-1;break;   
            case 10:exit(0);break;   
            case 11:diskShow();break;   
            }   
        }  
    }   
    return 0;   
}   

/* reads from keypress, doesn't echo */
int getch(void)
{
    char c;
    system("stty -echo");
    system("stty -icanon");
    c=getchar();
    system("stty icanon");
    system("stty echo");
    return c;
}

void userCreate()   
{   
    char c;   
    char userName[10];  
    int i;   
    if(used<MaxUser)   
    {   
        while (getchar () != '\n');   //清空流缓冲区
        printf("请输入用户名：");   
        for(i=0;c=getch();i++)   
        {   
            if(c=='\n') break;   
            else  
                userName[i]=c;   
            printf("%c",c);   
        }   
        userName[i]='\0';   
        for(i=0;i<used;i++)   
        {   
            if(!strcmp(userTable[i].userName,userName))   
            {   
                printf("\n");   
                printf("该用户名已存在,创建用户失败\n");   
                sleep(1);   
                return;   
            }   
        }   
        strcpy(userTable[used].userName,userName);   
        printf("\n");   
        printf("请输入密码："); 
        for(i=0;c=getchar();i++)   
        {   
            if(c=='\n') break;   
            else  
                userTable[used].password[i]=c;   
            printf("*");   
        }   
        userTable[userID].password[i]='\0';  
        printf("\n");   
        printf("创建用户成功\n");   
        used++;   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("创建用户失败，用户已达到上限\n");   
        sleep(1);//system("pause");   
    }   
}   
  
int login()   
{   
    char name[10],psw[10];   
    char c;   
    int i,times;   
    while (getchar () != '\n');   //清空流缓冲区
    printf("请输入用户名:"); 
    for(i=0;c=getchar();i++)   
    {   
        if(c=='\n') break;   
        else  
            name[i]=c;   
        printf("%c",c);   
    }   
    name[i]='\0';   
    for(i=0;i<used;i++)   
    {   
        if(!strcmp(userTable[i].userName,name))   
            break;   
    }   
    if(i==used)   
    {   
        printf("\n您输入的用户名不存在\n");   
        sleep(1);//system("pause");   
        return -1;   
    }   
    for(times=0;times<3;times++)   
    {   
        memset(psw,'\0',sizeof(psw));   
        printf("\n请输入密码:"); 
        for(i=0;c=getchar();i++)   
        {   
            if(c=='\n') break;   
            else  
                psw[i]=c;   
            printf("*");   
        }   
        printf("\n");   
        for(i=0;i<used;i++)   
        {   
            if(!strcmp(psw,userTable[i].password))   
            {   
                printf("用户登录成功\n");   
                sleep(1);//system("pause");   
                break;   
            }   
        }   
        if(i==used)   
        {   
            printf("您输入的密码错误，您还有%d次输入机会\n",2-times);   
            if(times==2) exit(0);   
        }   
        else break;   
    }   
    return i;   
}   
  
void initDisk()   
{   
    diskHead=(diskNode *)malloc(sizeof(diskNode));   
    diskHead->maxlength=MaxDisk;   
    diskHead->useFlag=0;   
    diskHead->start=0;   
    diskHead->next=NULL;   
}   
int requestDist(int startPostion,int maxLength)   
{   
    int flag=0;  //标记是否分配成功   
    diskNode *p,*q,*temp;   
    p=diskHead;   
    while(p)   
    {   
        if(p->useFlag==0&&p->maxlength>maxLength)   
        {   
            startPostion=p->start;   
            q=(diskNode *)malloc(sizeof(diskNode));   
            q->start=p->start;   
            q->maxlength=maxLength;   
            q->useFlag=1;   
            q->next=NULL;   
            diskHead->start=p->start+maxLength;   
            diskHead->maxlength=p->maxlength-maxLength;          
            flag=1;   
            temp=p;   
            if(diskHead->next==NULL) diskHead->next=q;   
            else  
            {   
                while(temp->next) temp=temp->next;   
                temp->next=q;   
            }   
            break;   
        }   
        p=p->next;   
    }   
    return flag;   
}   
  
void fileCreate(char fileName[],int length,char fileKind[])   
{   
    //int i,j;   
    time_t rawtime;   
    int startPos;   
    UFD *fileNode,*p;   
    for(p=userTable[userID].user->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
        {   
            printf("文件重名，创建文件失败\n");   
            sleep(1);//system("pause");   
            return;   
        }   
    }   
    if(requestDist(startPos,length))   
    {   
        fileNode=(UFD *)malloc(sizeof(UFD));   
        fileNode->file=(fileTableN *)malloc(sizeof(fileTableN));  //这一步必不可少，因为fileNode里面的指针也需要申请地址，否则fileNode->file指向会出错   
        strcpy(fileNode->file->fileName,fileName);   
        strcpy(fileNode->file->fileKind,fileKind);   
        fileNode->file->maxlength=length;   
        fileNode->file->strat=startPos;   
        fileNode->file->openFlag=false;   
        time(&rawtime);   
        fileNode->file->timeinfo=localtime(&rawtime);   
        fileNode->next=NULL;   
        if(userTable[userID].user->next==NULL)   
            userTable[userID].user->next=fileNode;   
        else  
        {   
            p=userTable[userID].user->next;   
            while(p->next) p=p->next;   
            p->next=fileNode;   
        }   
        printf("创建文件成功\n");   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("磁盘空间已满或所创建文件超出磁盘空闲容量，磁盘空间分配失败\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void freeDisk(int startPostion)   
{   
    diskNode *p;   
    for(p=diskHead;p!=NULL;p=p->next)   
    {   
        if(p->start==startPostion)   
            break;   
    }   
    p->useFlag=false;   
}   
  
void fileDel(char fileName[])   
{   
    UFD *p,*q,*temp;   
    q=userTable[userID].user;   
    p=q->next;   
    while(p)   
    {   
        if(!strcmp(p->file->fileName,fileName)) break;   
        else    
        {   
            p=p->next;   
            q=q->next;   
        }   
    }   
    if(p)   
    {   
        if(p->file->openFlag!=true)            //先判断是否有进程打开该文件   
        {   
            temp=p;   
            q->next=p->next;   
            freeDisk(temp->file->strat);  //磁盘空间回收   
            free(temp);   
            printf("文件删除成功\n");   
            sleep(1);//system("pause");   
        }   
        else  
        {   
            printf("该文件已被进程打开,删除失败\n");   
            sleep(1);//system("pause");   
        }   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void fileCat(char fileName[])   
{   
    int startPos,length;   
    int i,k=0;   
    UFD *p,*q;   
    q=userTable[userID].user;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)   
    {   
        startPos=p->file->strat;   
        length=p->file->length;   
        p->file->openFlag=true;    //文件打开标记   
        printf("*****************************************************\n");   
        //for(int i=startPos;k<length;i++,k++) 
        i=startPos;
        while(k<length)  
        {   
            if(i%50==0) printf("\n");  //一行大于50个字符换行   
            printf("%c",disk[i]);  
            i++;
            k++; 
        }   
        printf("\n\n*****************************************************\n");   
        printf("%s已被read进程打开,请用close命令将其关闭\n",p->file->fileName);   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void fileWrite(char fileName[])   
{   
    UFD *p,*q;   
    q=userTable[userID].user;   
    int i,k,startPos;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)   
    {   
        if(!strcmp(p->file->fileKind,"r"))      //判断文件类型   
        {   
            printf("该文件是只读文件,写入失败\n");   
            sleep(1);//system("pause");   
            return;   
        }   
        char str[500];   
        printf("please input content:\n");   
        //gets(str);
        scanf("%s",str);   
        startPos=p->file->strat;   
        p->file->openFlag=true;    //文件打开标记   
        p->file->length=strlen(str);   
        if(p->file->length>p->file->maxlength)   
        {   
            printf("写入字符串长度大于该文件的总长度,写入失败\n");   
            sleep(1);//system("pause");   
            return;   
        }   
        for(i=startPos,k=0;k<(int)strlen(str);i++,k++)   
            disk[i]=str[k];   
        printf("文件写入成功,请用close命令将该文件关闭\n");   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void fileFine(char fileName[])   
{   
    UFD *p,*q;   
    q=userTable[userID].user;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)   
    {   
        printf("********************************************\n");   
        printf("文件名：%s\n",p->file->fileName);   
        printf("文件长度：%d\n",p->file->maxlength);   
        printf("文件在存储空间的起始地址：%d\n",p->file->strat);   
        printf("文件类型：%s\n",p->file->fileKind);   
        printf("创建时间：%s\n",asctime(p->file->timeinfo));   
        printf("********************************************\n");   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void chmod(char fileName[],char kind[])   
{   
    UFD *p,*q;   
    q=userTable[userID].user;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)   
    {   
        strcpy(p->file->fileKind,kind);   
        printf("修改文件类型成功\n");   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void fileRen(char fileName[],char name[])   
{   
    UFD *p,*q;   
    q=userTable[userID].user;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)   
    {   
        while(q->next)   
        {   
            if(!strcmp(q->next->file->fileName,name))   
            {   
                printf("您输入的文件名已存在,重命名失败\n");   
                sleep(1);//system("pause");   
                return;   
            }   
            q=q->next;   
        }   
        strcpy(p->file->fileName,name);   
        printf("重命名成功\n");   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}   
  
void fileDir(char userName[])   
{   
    UFD *p;   
    int i,k;   
    for(i=0;i<MaxUser;i++)   
    {   
        if(!strcmp(userTable[i].userName,userName))   
        {   
            k=i;break;   
        }   
    }   
    if(i==MaxUser)   
    {   
        printf("没有找到该用户，请检查输入用户名是否正确\n");   
        //system("pause");   
        return;   
    }   
    else  
    {   
        p=userTable[k].user->next;   
        printf("***************************************************\n");   
        printf("文件名  文件长度  文件在磁盘的起始地址  文件类型  创建时间\n");   
        for(;p!=NULL;p=p->next)   
            printf("%s        %d              %d            %s   %s",p->file->fileName,   
            p->file->maxlength,p->file->strat,p->file->fileKind,asctime(p->file->timeinfo));   
        printf("****************************************************\n");   
        //system("pause");   
    }   
}   
  
void diskShow()   
{   
    diskNode *p;   
    int i=0,unusedDisk=0;   
    printf("***************************************************************************\n");   
    printf("  盘块号    起始地址       容量(byte)   是否已被使用\n");   
    for(p=diskHead;p!=NULL;p=p->next,i++)   
    {   
        if(p->useFlag==false) unusedDisk+=p->maxlength;   
        printf("   %d         %d              %d          %d     \n",i,p->start,p->maxlength,p->useFlag);   
    }   
    printf("***************************************************************************\n");   
    printf("磁盘空间总容量：512*1024Bytes  已使用：%dBytes   末使用：%dBytes\n\n",MaxDisk-unusedDisk,   
        unusedDisk);   
    sleep(1);//system("pause");   
}   
  
void fileClose(char fileName[])   
{   
    UFD *p,*q;   
    q=userTable[userID].user;   
    for(p=q->next;p!=NULL;p=p->next)   
    {   
        if(!strcmp(p->file->fileName,fileName))   
            break;   
    }   
    if(p)    
    {   
        p->file->openFlag=false;   
        printf("%s文件已关闭\n",p->file->fileName);   
        sleep(1);//system("pause");   
    }   
    else  
    {   
        printf("没有找到该文件,请检查输入的文件名是否正确\n");   
        sleep(1);//system("pause");   
    }   
}


