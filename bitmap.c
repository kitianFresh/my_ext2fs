#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include "./include/file.h"
#include "./include/head.h"
#define DISK "./disk"

/*位图的置位和复位，flag为1表示置位，flag为0表示复位,成功返回1,失败返回-1*/
static int set_bitmap(unsigned int  *bitmap, int size, int block_num, int flag);
/*搜索位图块中的空闲位,成功则返回块号，失败则返回-1*/
static int lookup_bitmap(unsigned int  *bitmap, int size);

unsigned int  m_inodemap[256];
unsigned int  m_blockmap[256];
struct ext2_super_block sb;

/*取block中的数据，读取放到ext2_super_block结构体指针指向的空间中*/
int get_superblock_data(struct ext2_super_block * sb){
	int fd;
	if((fd=open(DISK,O_RDONLY))==-1){
		perror("fail to open file\n");
		exit(EXIT_FAILURE);
	}
	/*为引导块预留1KB*/
	
	if(lseek(fd,1024,SEEK_SET)==-1){
		perror("file lseek\n");
		
		exit(EXIT_FAILURE);
	}
	/*读取超级块内容*/
	int num = read(fd,sb,sizeof(struct ext2_super_block));
	close(fd);
	return num;
}

/*向磁盘写入super_block数据*/
int write_superblock_data(struct ext2_super_block * sb){
	int fd;
	if((fd=open(DISK,O_RDWR))==-1){
		perror("fail to open file\n");
		exit(EXIT_FAILURE);
	}
	/*为引导块预留1KB*/
	
	if(lseek(fd,1024,SEEK_SET)==-1){
		perror("file lseek\n");
		exit(EXIT_FAILURE);
	}
	/*将超级块数据写入*/
	int num = write(fd,sb,sizeof(struct ext2_super_block));
	close(fd);
	return num;
}

/*测试完毕*/
int set_bitmap(unsigned int  *bitmap,int size, int block_num, int flag){
	int i = 0;
	if(block_num>=32*size || block_num < 0){
		perror("块无效！\n");
		return -1;
	}
	int index = block_num/32;
	int offset = block_num%32;
	if(flag==1){
		//printf("%d\t%#X\n",block_num,bitmap[index]);
		bitmap[index] |= (1<<(31-offset));
		//printf("%d\t%#X\n",block_num,bitmap[index]);
		return 1;
	}
	else {
		//printf("%d\t%#X\n",block_num,bitmap[index]);
		bitmap[index] &= (~(1<<(31-offset)));
		//printf("%d\t%#X\n",block_num,bitmap[index]);
		return 1;
	}
}

/*测试完毕*/
#define mask 0x80000000
int lookup_bitmap(unsigned int *bitmap, int size){
	int i = 0;
	for(;i<size;i++){
		int j=0;
		for(;j<=31;j++){
			if(!((bitmap[i]<<j) & mask)){
				//printf("bitmap[%d]:%#X\n",i,bitmap[i]);
				return i*32+j;
			}
		}
	}
	return -1;
}

/*测试完毕*/
int get_inodemap(char *m_inodemap, int size){
	int fd;
	if((fd=open(DISK,O_RDONLY))==-1){
		perror("fail to read inode_bitmap!\n");
		return -1;
	}
	if(lseek(fd,1024+1024,SEEK_SET)==-1){
		perror("fial to lseek\n");
		return -1;
	}
	/*读节点位图*/
	if(read(fd,m_inodemap,INODE_BITMAP_SIZE) == -1) return -1;
	close(fd);
	return 1;
}

/*测试完毕*/
int put_inodemap(char *m_inodemap,int size){
	int fd;
	if((fd=open(DISK,O_RDWR))==-1){
		perror("fail to open file!\n");
		return -1;
	}
	if(lseek(fd,1024+1024,SEEK_SET)==-1){
		perror("fail to lseek!\n");
		return -1;
	}
	/*将节点位图写回磁盘*/
	if(write(fd,m_inodemap,INODE_BITMAP_SIZE) == -1) return -1;
	close(fd);
	return 1;
}

/*测试完毕*/
int get_blockmap(char *m_blockmap,int size){
	int fd;
	if((fd=open(DISK,O_RDONLY))==-1){
		perror("fail to open file!\n");
		return -1;
	}
	if(lseek(fd,1024+1024+1024,SEEK_SET)==-1){
		perror("fail to lseek!\n");
		return -1;
	}
	/*读数据块位图*/
	if(read(fd,m_blockmap,BLOCK_BITMAP_SIZE) == -1) return -1;
	close(fd);
	return 1;
}

/*测试完毕*/
int put_blockmap(char *m_blockmap,int size){
	int fd;
	if((fd=open(DISK,O_RDWR))==-1){
		perror("fail to open file!\n");
		return -1;
	}
	if(lseek(fd,1024+1024+1024,SEEK_SET)==-1){
		perror("fail to lseek!\n");
		return -1;
	}
	/*将数据块位图写回磁盘*/
	if(write(fd,m_blockmap,BLOCK_BITMAP_SIZE) == -1) return -1;
	close(fd);
	return 1;
}

/*测试完毕*/
int free_block(int block_num){
	if(get_blockmap((char*)m_blockmap,1024) == -1) return -1;
	if(set_bitmap(m_blockmap,256,block_num,0) == -1) return -1; 
	if(put_blockmap((char*)m_blockmap,1024) == -1) return -1;
	/*同步超级块*/	
	get_superblock_data(&sb);
	sb.s_free_blocks_count++;
	//printf("free_block:sb.s_free_blocks_count=%d\n",sb.s_free_blocks_count);
	write_superblock_data(&sb);
	return 1;
}

/*测试完毕*/
int new_block(){
	if(get_blockmap((char*)m_blockmap,1024) == -1) return -1;
	int block_num;
	if((block_num=lookup_bitmap(m_blockmap,256)) == -1) return -1;
	if(set_bitmap(m_blockmap,256,block_num,1) == -1) return -1;
	if(put_blockmap((char*)m_blockmap,1024) == -1) return -1;
	/*同步超级块*/	
	get_superblock_data(&sb);
	sb.s_free_blocks_count--;
	//printf("new_block:sb.s_free_blocks_count=%d\n",sb.s_free_blocks_count);
	write_superblock_data(&sb);
	return block_num;
}

/*测试完毕*/
int free_inode(int inode_num){
	if(get_inodemap((char*)m_inodemap,1024) == -1) return -1;
	if(set_bitmap(m_inodemap,256,inode_num,0) == -1) return -1; 
	if(put_inodemap((char*)m_inodemap,1024) == -1) return -1;
	/*同步超级块*/	
	get_superblock_data(&sb);
	sb.s_free_inodes_count++;
	//printf("free_inode:sb.s_free_inodes_count=%d\n",sb.s_free_inodes_count);
	write_superblock_data(&sb);
	return 1;
}

/*测试完毕*/
int new_inode(){
	if(get_inodemap((char*)m_inodemap,1024) == -1) return -1;
	int inode_num;
	if((inode_num=lookup_bitmap(m_inodemap,256)) == -1) return -1;
	if(set_bitmap(m_inodemap,256,inode_num,1) == -1) return -1;
	if(put_inodemap((char*)m_inodemap,1024) == -1) return -1;
	/*同步超级块*/	
	get_superblock_data(&sb);
	sb.s_free_inodes_count--;
	//printf("free_inode:sb.s_free_inodes_count=%d\n",sb.s_free_inodes_count);
	write_superblock_data(&sb);
	return inode_num;
}

