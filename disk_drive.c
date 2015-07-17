#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include "./include/file.h"
#include "./include/head.h"
#define DISK "./disk"

/*取block中的数据，index为block的编号，读取的字节放到buffer指向的空间中*/

int get_block_data(unsigned int index,void *buffer){
	if(index<0  || !buffer)
		return 0;
	unsigned int file_location=GUIDE_SIZE+SUPER_SIZE+INODE_BITMAP_SIZE+
		BLOCK_BITMAP_SIZE+INODE_SIZE*INODE_NUM+index*BLOCK_SIZE;
	int fd;
	if((fd=open(DISK,O_RDONLY))==-1){
		perror("disk_drive: fail to open file");
		exit(EXIT_FAILURE);
	}
	if(lseek(fd,file_location,SEEK_SET)==-1){
		perror("disk_drive get: fail lseek");
		exit(EXIT_FAILURE);
	}
	int count=read(fd,buffer,BLOCK_SIZE);
	close(fd);
	return count;
}

/*向block中写入数据，index为block的编号*/

int write_block_data(unsigned int index,const void *buffer){
	if(index<0 || !buffer)
		return 0;
	unsigned int file_location=GUIDE_SIZE+SUPER_SIZE+INODE_BITMAP_SIZE+
		BLOCK_BITMAP_SIZE+INODE_SIZE*INODE_NUM+index*BLOCK_SIZE;
	int fd;
	if((fd=open("./disk",O_RDWR))==-1){
		perror("disk_drive: fail to open file");
		exit(EXIT_FAILURE);
	}
	if(lseek(fd,file_location,SEEK_SET)==-1){
		perror("disk_drive put: fail lseek");
		exit(EXIT_FAILURE);
	}
	int count=write(fd,buffer,BLOCK_SIZE);
	close(fd);
	return count;
}

/*取ext2_inode中的数据，index为ext2_inode的磁盘编号，*/
int get_inode_data(unsigned int index, void *m_inode){
	int fd;
	int file_location=GUIDE_SIZE+SUPER_SIZE+INODE_BITMAP_SIZE+
		BLOCK_BITMAP_SIZE+INODE_SIZE*index;
	if(index<0) return -1;
	if((fd=open(DISK,O_RDONLY)) == -1){
		perror("disk_drive: fail to open file\n");
		exit(EXIT_FAILURE);
	}
	if(lseek(fd,file_location,SEEK_SET) == -1){
		perror("disk_drive:fail lssek\n");
		exit(EXIT_FAILURE);
	}
	int num = read(fd,m_inode,INODE_SIZE);
	((struct inode*)m_inode)->i_number = index;
	close(fd);
	return num;
}

/*向inode中写入数据，index为inode的磁盘编号*/
int write_inode_data(unsigned int index, void *m_inode ){
	int fd;
	int file_location=GUIDE_SIZE+SUPER_SIZE+INODE_BITMAP_SIZE+
		BLOCK_BITMAP_SIZE+INODE_SIZE*index;
	if(index<0) return -1;
	if((fd=open(DISK,O_RDWR)) == -1){
		perror("disk_drive: fail to open file\n");
		exit(EXIT_FAILURE);
	}
	if(lseek(fd,file_location,SEEK_SET) == -1){
		perror("disk_drive:fail lseek\n");
		exit(EXIT_FAILURE);
	}
	int num = write(fd,m_inode,INODE_SIZE);
	close(fd);
	return num;
}
/*int main(){
	//char *buffer="hello world";
	char df[1024];
	//write_block_data(10,buffer);
	get_block_data(10,df);
	int count=printf("%.1024s\n",df);
	printf("%d\n",count);
	return 0;
}*/
