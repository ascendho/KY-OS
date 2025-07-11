#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
struct Game   //游戏信息
{
    int Round;
    long Type;
};
void result_send(int num)     //发送出拳信息
{
    struct Game game;
    game.Type = 1;
    game.Round = rand() % 3;
    msgsnd(num, &game, sizeof(int), 0);
}
int result_announce(int a, int b)    //出拳结果的判断
{
    if ((a + 1 == b) || (a - 3 == b))
        return -1;    //a胜b
    else if (a == b)
        return 0;    //ab平局
    else
        return 1;    //a负b
}
void writeFile(int *result_list, int len)   //将每盘的结果存入文件
{
    int count_A = 0; 
    int count_B = 0;
    int pingju=0;
    FILE *fin;
    if( (fin = fopen( "result.txt", "w" )) == NULL ) 
        printf( "This file wasn't opened" );

    int i;
    for (i = 0; i < len ; i++)
    {
        switch(result_list[i])
        {
            case -1 :{
                count_A++;
                fprintf(fin, "NO.%d:A win\n", i + 1);
                printf("NO.%d:A win\n", i + 1);
                break;
            }
            case 0 : {
                pingju++;
                fprintf(fin, "NO.%d:end in a draw\n", i + 1);
                printf("NO.%d:end in a draw\n", i + 1);
                break;
            }
            case 1 : {
                count_B++;
                fprintf(fin, "NO.%d:B win\n", i + 1);
                printf("NO.%d:B win\n", i + 1);
                break;
            }
        }
    }

    printf("\nThe final result is A win:%ds \nB win:%ds \nend in a draw %ds\n",count_A,count_B,pingju);
    fprintf(fin, "\nThe final result is A win:%ds \nB win:%ds \nend in a draw %ds\n",count_A,count_B,pingju);
    fclose(fin);
}
int main()
{
    int times; 
    int key1 = 1234;
    int key2 = 5678;
    int *result_list;
    pid_t  pid1, pid2;  
    int msgid1,msgid2;
    msgid1 = msgget(key1, IPC_CREAT | 0666); //创建消息队列
    if(msgid1 == -1)
    {
        fprintf(stderr, "failed with error");
        exit(EXIT_FAILURE);
    }
    msgid2 = msgget(key2, IPC_CREAT | 0666); //创建消息队列
    if(msgid2 == -1)
    {
        fprintf(stderr, "failed with error");
        exit(EXIT_FAILURE);
    }
    printf("Game start,please input rounds:");
    scanf("%d", &times);
    result_list=(int*)malloc(times*sizeof(int));
    int i;
    for (i = 0; i < times; i++)
    {
        pid1 = fork();   //创建选手1
        if (pid1 == 0) 
        {
            srand((unsigned)time(0) * 3000 ); //以时间为种子
            result_send(msgid1);  //生成选手1的出拳信息并发送到信息队列
            exit(-1);
        }
        pid2 = fork();   //创建选手2
        if (pid2 == 0) 
        {
            srand((unsigned)time(NULL)*i ); //以时间为种子
            result_send(msgid2);  //生成选手2的出拳信息并发送到信息队列
            exit(-1);
        }

        if (pid1 < 0 || pid2 < 0)
        {
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else
        {
            wait(NULL);
            wait(NULL);
            struct Game game1;
            struct Game game2;
//printf("wait ok.\n");
	   //从消息队列中取得选手1的出拳信息
            msgrcv(msgid1, &game1, sizeof(game1) - sizeof(long), 0, 0);
//printf("rcv1 ok.\n");
	    //从消息队列中取得选手2的出拳信息
            msgrcv(msgid2, &game2, sizeof(game2) - sizeof(long), 0, 0); 
	    //评判出拳结果
//printf("rcv2 ok.\n");
            int j = result_announce(game1.Round, game2.Round);
            //result_list[i] = result_announce(game1.Round, game2.Round); 
            result_list[i] = j;
        }
    }
//printf("end ok.\n");
    //将比赛结果写入文件
    writeFile(result_list, times);
    //删除消息队列
/*    if (msgctl(msgid1, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    }
    if (msgctl(msgid2, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
    }
 */   exit(EXIT_SUCCESS);
}

