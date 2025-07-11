#include <linux/module.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
extern unsigned long pfcount;
static int my_proc_show(struct seq_file* m, void* v){
    seq_printf(m, "The pfcount is %ld and jiffies is %ld!\n", pfcount,jiffies);
    return 0;
}
static int my_proc_open(struct inode* inode, struct file* file){
    return single_open(file, my_proc_show, NULL);
}
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = my_proc_open,
    .release = single_release,
    .read = seq_read,
    .llseek = seq_lseek,
};
static int __init my_init(void){
    struct proc_dir_entry* file = proc_create("readpfcount",0x0644, NULL, &my_fops);
    if (!file) {
        printk("proc_create failed.\n");
        return -ENOMEM;
    }
    return 0;
}
static void __exit my_exit(void){
    remove_proc_entry("readpfcount", NULL);
}
module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");