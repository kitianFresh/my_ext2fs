/*把disk文件格式化为ext2*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<fcntl.h>
#include"./include/file.h"
#include"./include/head.h"
#define DISK "./disk"

void mkfs_ext2(){
	char buf[1024];
	int fd;
	if((fd=open(DISK,O_WRONLY|O_CREAT,0666))==-1){
		perror("fail to open file");
		exit(EXIT_FAILURE);
	}
	/*为引导块预留1KB*/
	if(lseek(fd,1024,SEEK_CUR)==-1){
		perror("file lseek");
		exit(EXIT_FAILURE);
	}
	/*将超级块数据写入*/
	struct ext2_super_block super_block;
	memset(&super_block,'\0',sizeof(struct ext2_super_block));
	super_block.s_inodes_count=8*1024;
	super_block.s_blocks_count=8*1024;
	super_block.s_r_blocks_count=8;
	super_block.s_free_blocks_count=8*1024-8;
	super_block.s_free_inodes_count=8*1024;
	super_block.s_first_data_block=1;
	super_block.s_log_block_size=0;/*逻辑块为1024B*/
	time_t timenow;
	time(&timenow);/*获取系统当前的时间戳*/
	super_block.s_mtime=timenow;
	super_block.s_wtime=timenow;
	super_block.s_mnt_count=0;
	super_block.s_max_mnt_count=10;
	#define EXT2_VALID_FS 1 /*干净未挂载状态*/
	#define EXT2_ERROR_FS 2 /*文件系统处在挂载状态*/
	super_block.s_state=EXT2_VALID_FS;
	super_block.s_errors=1;
	super_block.s_lastcheck=timenow;
	super_block.s_checkinterval=10;
	super_block.s_first_ino=0;
	super_block.s_inode_size=128;
	if(write(fd,&super_block,sizeof(struct ext2_super_block))==-1){
		perror("write super block");
		exit(EXIT_FAILURE);
	}
	int i;
	memset(buf,'\0',sizeof(buf));
	for(i=0;i<1+1+1024+8*1024;i++){
		if(write(fd,buf,sizeof(buf))==-1){
			perror("write inode block bitmap");
			exit(EXIT_FAILURE);
		}
	}
	close(fd);
}
