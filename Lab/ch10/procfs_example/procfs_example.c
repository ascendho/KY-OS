#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/seq_file.h>
#include <linux/fs.h>

#define MODULE_VERS "1.0"
#define MODULE_NAME "procfs_example"
#define FOOBAR_LEN 8
struct fb_data_t{
	char name[FOOBAR_LEN+1];
	char value[FOOBAR_LEN+1];
};
static struct proc_dir_entry *example_dir, *foo_file, *bar_file, *jiffies_file, *symlink;
static int major=255;
static int minor=0;
struct fb_data_t foo_data, bar_data;
int foo_len,foo_temp,bar_len,bar_temp;
int jiff_temp=-1;
char tempstr[FOOBAR_LEN*2+5];

//===========jiffies文件操作函数=========
static ssize_t read_jiffies_proc(struct file *filp,char __user *buf,size_t count,loff_t *offp ){
	printk(KERN_INFO"count=%d  jiff_temp=%d\n", count, jiff_temp);
	char tempstring[100]="";
	if (jiff_temp!=0)
		jiff_temp=sprintf(tempstring, "jiffies=%ld\n", jiffies);	//jiffies为系统启动后经过的时间戳
	if (count>jiff_temp)
		count=jiff_temp;
	jiff_temp=jiff_temp-count;
	printk(KERN_INFO"count=%d  jiff_temp=%d\n", count, jiff_temp);
	copy_to_user(buf, tempstring, count);
	if (count==0)
		jiff_temp=-1;	//读取结束后temp变回-1
	return count;
}
static const struct file_operations jiffies_proc_fops={
	.read=read_jiffies_proc
};
//=============foo文件操作函数===========
static ssize_t read_foo_proc(struct file *filp,char __user *buf,size_t count,loff_t *offp ) {
	printk(KERN_INFO"count=%d\n", count);
	//调整count与temp的值，具体过程自行查看printk输出的信息来分析
	if (count>foo_temp)
		count=foo_temp;
	foo_temp=foo_temp-count;
	//拼接tempstr字符串
	strcpy(tempstr, foo_data.name);
	strcat(tempstr, "='");
	strcat(tempstr, foo_data.value);
	strcat(tempstr, "'\n");
	printk(KERN_INFO"count=%d  length(tempstr)=%d\n", count, strlen(tempstr));
	//向用户空间写入tempstr
	copy_to_user(buf, tempstr, count);
	//如果count=0，读取结束，temp回归为原来的值
	if (count==0)
		foo_temp=foo_len+4;
	return count;
}
static ssize_t write_foo_proc(struct file *filp,const char __user *buf,size_t count,loff_t *offp ){
	int len;
	if (count>FOOBAR_LEN)
		len=FOOBAR_LEN;
	else
		len=count;
	//将数据写入foo_data的value字段
	if (copy_from_user(foo_data.value, buf, len))
		return -EFAULT;
	foo_data.value[len-1]='\0';	//减1是因为除去输入的回车
	//更新len与temp值
	foo_len=strlen(foo_data.name)+strlen(foo_data.value);
	foo_temp=foo_len+4;
	return len;
}
static const struct file_operations foo_proc_fops={
	.read=read_foo_proc,
	.write=write_foo_proc
};
//===============bar文件操作函数============
static ssize_t read_bar_proc(struct file *filp,char __user *buf,size_t count,loff_t *offp ) {	
	printk(KERN_INFO"count=%d\n", count);
	if (count>bar_temp)
		count=bar_temp;
	bar_temp=bar_temp-count;
	strcpy(tempstr, bar_data.name);
	strcat(tempstr, "='");
	strcat(tempstr, bar_data.value);
	strcat(tempstr, "'\n");
	printk(KERN_INFO"count=%d  length(tempstr)=%d\n", count, strlen(tempstr));
	copy_to_user(buf, tempstr, count);
	if (count==0)
		bar_temp=bar_len+4;
	return count;
}
static ssize_t write_bar_proc(struct file *filp,const char __user *buf,size_t count,loff_t *offp ){
	int len;
	if (count>FOOBAR_LEN)
		len=FOOBAR_LEN;
	else
		len=count;
	if (copy_from_user(bar_data.value, buf, len))
		return -EFAULT;
	bar_data.value[len-1]='\0';
	bar_len=strlen(bar_data.name)+strlen(bar_data.value);
	bar_temp=bar_len+4;
	return len;
}
static const struct file_operations bar_proc_fops={
	.read=read_bar_proc,
	.write=write_bar_proc
};
//===============模块init函数=============
static int __init init_procfs_example(void){
	int rv=0;
	
	//=========创建目录===========
	example_dir=proc_mkdir(MODULE_NAME, NULL);
	if (example_dir==NULL){
		rv=-ENOMEM;
		goto out;
	}
	//======创建 jiffies(只读)=====
	jiffies_file=proc_create("jiffies", 0444, example_dir, &jiffies_proc_fops);
	if (jiffies_file==NULL){
		rv=-ENOMEM;
		goto no_jiffies;
	}
	//=============创建 foo==============
	strcpy(foo_data.name, "foo");
	strcpy(foo_data.value, "foo");
	foo_len=strlen(foo_data.name)+strlen(foo_data.value);
	foo_temp=foo_len+4;		//加4是因为拼接tempstr字符串时多了=‘’\n”这四个字符
	foo_file=proc_create("foo", 0, example_dir, &foo_proc_fops);
	if (foo_file==NULL){
		rv=-ENOMEM;
		goto no_foo;
	}
	//=============创建 bar===============
	strcpy(bar_data.name, "bar");
	strcpy(bar_data.value, "bar");
	bar_len=strlen(bar_data.name)+strlen(bar_data.value);
	bar_temp=bar_len+4;
	bar_file=proc_create("bar", 0, example_dir, &bar_proc_fops);
	if (bar_file==NULL){
		rv=-ENOMEM;
		goto no_bar;
	}
	//===========创建 symlink==============
	symlink=proc_symlink("jiffies_too", example_dir, "jiffies");
	if (symlink==NULL){
		rv=-ENOMEM;
		goto no_symlink;
	}
	//=============all okay=================
	printk(KERN_INFO"%s%s initialised\n", MODULE_NAME, MODULE_VERS);
	return 0;

no_symlink:
	remove_proc_entry("jiffies_too", example_dir);
no_bar:
	remove_proc_entry("bar",example_dir);
no_foo:
	remove_proc_entry("foo", example_dir);
no_jiffies:
	remove_proc_entry("jiffies", example_dir);
out:
	return rv;
}
//==============模块cleanup函数==============
static void __exit cleanup_procfs_example(void){
	remove_proc_entry("jiffies_too", example_dir);
	remove_proc_entry("bar", example_dir);
	remove_proc_entry("foo", example_dir);
	remove_proc_entry("jiffies", example_dir);
	remove_proc_entry(MODULE_NAME, NULL);
	printk(KERN_INFO"%s%s removed\n", MODULE_NAME, MODULE_VERS);
}
MODULE_LICENSE("GPL");
module_init(init_procfs_example);
module_exit(cleanup_procfs_example);
MODULE_DESCRIPTION("proc filesystem example");
