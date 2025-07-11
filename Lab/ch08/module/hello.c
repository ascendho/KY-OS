#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/unistd.h>
#include <linux/sched.h>
MODULE_LICENSE("Dual BSD/GPL");
#define SYS_CALL_TABLE_ADDRESS 0xffffffffabe001a0  //sys_call_table对应的地址
#define NUM 223  //系统调用号为223
int orig_cr0;  //用来存储cr0寄存器原来的值
unsigned long *sys_call_table_my=0;
static int(*anything_saved)(void);  //定义一个函数指针，用来保存一个系统调用
static int clear_cr0(void) //使cr0寄存器的第17位设置为0（内核空间可写）
{
    unsigned int cr0=0;
    unsigned int ret;
    asm volatile("movq %%cr0,%%rax":"=a"(cr0));//将cr0寄存器的值移动到eax寄存器中，同时输出到cr0变量中
    ret=cr0;
    cr0&=0xfffffffffffeffff;//将cr0变量值中的第17位清0,将修改后的值写入cr0寄存器
    asm volatile("movq %%rax,%%cr0"::"a"(cr0));//将cr0变量的值作为输入，输入到寄存器eax中，同时移动到寄存器cr0中
    return ret;
}
static void setback_cr0(int val) //使cr0寄存器设置为内核不可写
{
    asm volatile("movq %%rax,%%cr0"::"a"(val));
}
asmlinkage long sys_mycall(void) //定义自己的系统调用 
{   
    printk("模块系统调用-当前pid：%d，当前comm:%s\n",current->pid,current->comm);
    printk("hello world!\n");
    return current->pid;    
}
static int __init call_init(void)
{
    sys_call_table_my=(unsigned long*)(SYS_CALL_TABLE_ADDRESS);
    printk("call_init......\n");
    anything_saved=(int(*)(void))(sys_call_table_my[NUM]);//保存系统调用表中的NUM位置上的系统调用
    orig_cr0=clear_cr0();//使内核地址空间可写
    sys_call_table_my[NUM]=(unsigned long) &sys_mycall;//用自己的系统调用替换NUM位置上的系统调用
    setback_cr0(orig_cr0);//使内核地址空间不可写
    return 0;
}
static void __exit call_exit(void)
{
    printk("call_exit......\n");
    orig_cr0=clear_cr0();
    sys_call_table_my[NUM]=(unsigned long)anything_saved;//将系统调用恢复
    setback_cr0(orig_cr0);
}
module_init(call_init);
module_exit(call_exit);
