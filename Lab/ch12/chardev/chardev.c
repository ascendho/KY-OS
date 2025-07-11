#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#define SUCCESS  0
#define DEVICE_NAME  "chardev"
#define BUF_LEN  80
static int Major;
static int Device_Open =0;
static char msg[BUF_LEN];
static char *msg_Ptr;
static int device_open(struct inode *inode,struct file *file);
static int device_release(struct inode *inode,struct file *file);
static ssize_t device_read(struct file *filp,char *buffer,size_t length,loff_t *offset);
static ssize_t device_write(struct file *filp,const char *buff,size_t length,loff_t *off);
static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};
//打开设备
static int device_open(struct inode *inode,struct file *file)
{
	static int counter = 0;
	if(Device_Open)
		return -EBUSY;
	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world\n", counter++);
	msg_Ptr =msg;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}
//释放设备
static int device_release(struct inode *inode,struct file *file)
{
	Device_Open--;
	module_put(THIS_MODULE);
	return 0;
}
//读设备
static ssize_t device_read(struct file *filp,char *buffer,size_t length,loff_t*offset)
{
	if(*msg_Ptr == 0)
		return 0;
	copy_to_user(buffer, msg_Ptr, length);
	return strlen(msg);
}
//写设备
static ssize_t device_write(struct file *filp,const char *buff,size_t length,loff_t *off)
{
	printk("<1> Sorry this operation isn't supported\n "); //未实现write功能
	return -EINVAL;
}
//初始化字符设备
int init_chardev_module(void)
{
	Major = register_chrdev(0,DEVICE_NAME,&fops);
	if(Major <0){
		printk("Registering the character device failed with %d \n ", Major);
		return Major;
	}
	printk("<1> I was assigned major number %d ", Major);
	printk("<1> the drive, create a dev file");
	printk("<1> mknod /dev/hello c %d 0.\n" ,Major);
	printk("<1> I was assigned major number %d ", Major);
	printk("<1> the device file\n" );
	printk("<1> Remove the file device and module when done\n");
	return 0;
}
//关闭字符设备
void exit_chardev_module(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
}
MODULE_LICENSE("Dual BSD/GPL");
module_init(init_chardev_module);
module_exit(exit_chardev_module);