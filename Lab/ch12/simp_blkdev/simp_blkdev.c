#include <linux/module.h>
#include <linux/blkdev.h>

#define SIMP_BLKDEV_DISKNAME "simp_blkdev"
#define SIMP_BLKDEV_DEVICEMAJOR COMPAQ_SMART2_MAJOR
#define SIMP_BLKDEV_BYTES (50*1024*1024)
#define SECTOR_SIZE_SHIFT 9
static struct gendisk *simp_blkdev_disk;
static struct block_device_operations simp_blkdev_fops = {
	.owner = THIS_MODULE,
};
static struct request_queue *simp_blkdev_queue;
unsigned char simp_blkdev_data[SIMP_BLKDEV_BYTES];
/*磁盘块设备数据请求处理函数*/
static void simp_blkdev_do_request(struct request_queue *q){
	struct request *req;	//正在处理的请求队列中的请求
	struct bio *req_bio;	//当前请求的bio
	struct bio_vec *bvec;	//当前请求bio的段
	char *disk_mem;		//需要读/写的磁盘区域
	char *buffer;		//磁盘块设备的请求在内存中的缓冲区
	int i = 0;
	while((req = blk_fetch_request(q)) != NULL){
		//判断当前req是否合法
		if((blk_rq_pos(req)<<SECTOR_SIZE_SHIFT)+blk_rq_bytes(req)>SIMP_BLKDEV_BYTES){
			printk(KERN_ERR SIMP_BLKDEV_DISKNAME": bad request:block=%llu, count=%u \n", (unsigned long long)blk_rq_pos(req), blk_rq_sectors(req));
			blk_end_request_all(req, -EIO);
			continue;
		}
		//获取需要操作的内存位置
		disk_mem = simp_blkdev_data + (blk_rq_pos(req) << SECTOR_SIZE_SHIFT);
		req_bio = req->bio; //获取当前请求的bio
		switch(rq_data_dir(req)){
		case READ:
			while(req_bio != NULL){
				for(i=0;i<req_bio->bi_vcnt;i++){
					bvec = &(req_bio->bi_io_vec[i]);
					buffer = kmap(bvec->bv_page) + bvec->bv_offset;
					memcpy(buffer,disk_mem,bvec->bv_len);
					kunmap(bvec->bv_page);
					disk_mem += bvec->bv_len;
				}
				req_bio = req_bio->bi_next;
			}
			__blk_end_request_all(req,0);
			break;
		case WRITE:
			while(req_bio != NULL){
				for(i=0;i<req_bio->bi_vcnt;i++){
					bvec = &(req_bio->bi_io_vec[i]);
					buffer = kmap(bvec->bv_page) + bvec->bv_offset;
					memcpy(disk_mem,buffer,bvec->bv_len);
					kunmap(bvec->bv_page);
					disk_mem += bvec->bv_len;
				}
				req_bio = req_bio->bi_next;
			}
			__blk_end_request_all(req,0);
		default:
			break;
		}
	}
}
/* 模块入口函数 */
static int __init simp_blkdev_init(void){
	int ret;
	//1.添加设备之前，先申请设备的资源
	simp_blkdev_disk = alloc_disk(1);
	if(!simp_blkdev_disk){
		ret = -ENOMEM;
		goto err_alloc_disk;
	}
	//2.设置设备相关属性（设备名，设备号，请求队列）
	strcpy(simp_blkdev_disk->disk_name,SIMP_BLKDEV_DISKNAME);
	simp_blkdev_disk->major = SIMP_BLKDEV_DEVICEMAJOR;
	simp_blkdev_disk->first_minor = 0;
	simp_blkdev_disk->fops = &simp_blkdev_fops;
	//将块设备请求处理函数的地址传入blk_init_queue函数，初始化一个请求队列
	simp_blkdev_queue = blk_init_queue(simp_blkdev_do_request, NULL);
	if(!simp_blkdev_queue){
		ret = -ENOMEM;
		goto err_init_queue;
	}
	simp_blkdev_disk->queue = simp_blkdev_queue;
	set_capacity(simp_blkdev_disk, SIMP_BLKDEV_BYTES>>9);
	//3.入口处添加磁盘块设备
	add_disk(simp_blkdev_disk);
	return 0;
	err_alloc_disk:
		return ret;
	err_init_queue:
		return ret;
}
/* 模块出口函数 */
static void __exit simp_blkdev_exit(void){
	del_gendisk(simp_blkdev_disk);	//释放磁盘块设备
	put_disk(simp_blkdev_disk);	//释放申请的设备资源
	blk_cleanup_queue(simp_blkdev_queue);	//清除请求队列
}
MODULE_LICENSE("GPL");
module_init(simp_blkdev_init);	
module_exit(simp_blkdev_exit);	