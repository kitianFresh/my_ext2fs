/*实现 file_operations 中定义的函数指针*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include "./include/file.h"
#include "./include/head.h"
#define DISK "./disk"

/*int my_read(struct inode *inode,struct file *file,char *buffer,int count){
	if(!inode || !file || !buffer || !count)
		exit(EXIT_FAILURE);
	int i;
	int block_index=file->f_pos/BLOCK_SIZE;//block块号
	int block_offset=file->f_pos%BLOCK_SIZE;//block内部偏移
	int block_num=(count+block_offset)/BLOCK_SIZE+1;
	char * my_buffer=(char*)malloc(block_num*BLOCK_SIZE);
	if(!my_buffer){
		perror("file_operations : fail malloc");
		exit(EXIT_FAILURE);
	}
	if(block_num<=12){
		for(i=0;i<block_num;i++){
			get_block_data(inode->ext2_inode.i_block[i],my_buffer+i*BLOCK_SIZE);
		}	
	}
	//一次间接块指针，可以指向256个block
	else if(block_num>12 && block_num<=268){
		unsigned int *first_buffer=(unsigned int *)malloc(BLOCK_SIZE);
		if(!first_buffer){
			perror("file_operations : fail malloc");
			exit(EXIT_FAILURE);
		}
		get_block_data(inode->ext2_inode.i_block[12],first_buffer);
		for(i=0;i<block_num-12;i++){
			get_block_data(first_buffer[i],my_buffer+12*BLOCK_SIZE+i*
							BLOCK_SIZE);
		}
		free(first_buffer);
	}
	for(i=0;i<count;i++){
		buffer[i]=my_buffer[block_offset+i];
	}
	free(my_buffer);
	file->f_pos+=count;
	return count;
}


int my_write(struct inode* inode,struct file* file,const char* buffer,int count){
	int save_block=file->f_pos/BLOCK_SIZE;
	char *my_buffer=(char*)malloc(BLOCK_SIZE);
	if(!my_buffer){
		perror("file operation : file malloc");
		exit(EXIT_FAILURE);	
	}	
	get_block_data(inode->ext2_inode.i_block[save_block],my_buffer);
	int i;
	for(i=save_block;i<12;i++){
		free_block(inode->ext2_inode.i_block[i]);
	}
	i=file->f_pos-save_block*BLOCK_SIZE;
	int buffer_index;
	unsigned int temp_block;
	for(buffer_index=0;buffer_index<count && buffer_index<BLOCK_SIZE-i;buffer_index++,i++){
		my_buffer[i]=buffer[buffer_index];
	}
	int char_count=printf("%.1024s\n",my_buffer);
	printf("%d\n",char_count);
	temp_block=new_block();
	printf("temp_block %d\n",temp_block);
	write_block_data(temp_block,my_buffer);
	inode->ext2_inode.i_block[++save_block]=temp_block;
	if(buffer_index<count){
		for(;buffer_index<count;buffer_index+=BLOCK_SIZE){
			temp_block=new_block();
			printf("temp_block %d\n",temp_block);
			write_block_data(temp_block,buffer+buffer_index);
			inode->ext2_inode.i_block[++save_block]=temp_block;
		}
	}
	free(my_buffer);
	file->f_pos+=count;
	inode->ext2_inode.i_size=file->f_pos;
	return count;
}

*/
/*定位文件当前读写指针在所在的位置在inode.ext2_inode.i_block[15]中的位置,以及块内偏移
  f_pos为文件指针，用 index 和 offset 返回位置和偏移*/
static void find_position(unsigned int f_pos,unsigned int *index,unsigned int *offset){
	*index=f_pos/BLOCK_SIZE;
	*offset=f_pos%BLOCK_SIZE;
}

int my_read(struct inode *inode,struct file *file,char *buffer,int count){
	if(!inode || !file || !buffer || !count)
		return 0;
	int i;
	unsigned int block_index,block_offset;
	find_position(file->f_pos,&block_index,&block_offset);
	//printf("block_index %d  block_offset %d\n",block_index,block_offset);
	int block_num=(count+block_offset)/BLOCK_SIZE+1;
	//printf("block_num %d\n",block_num);
	char *my_buffer=(char*)malloc(block_num*BLOCK_SIZE);
	if(!my_buffer){
		perror("file_operations : fail malloc");
		exit(EXIT_FAILURE);
	}
	for(i=block_index;i<block_num+block_index;i++){
		//printf("read block index %d\n",inode->ext2_inode.i_block[i]);
		get_block_data(inode->ext2_inode.i_block[i],my_buffer+i*BLOCK_SIZE);
	}	
	for(i=0;i<count;i++){
		buffer[i]=my_buffer[block_offset+i];
	}
	free(my_buffer);
	file->f_pos+=count;
	return count;
}


int my_lseek(struct inode* inode,struct file* file,int off,int flag){
	if(flag==0){
		file->f_pos=off;
	}
	else if(flag==1){
		file->f_pos+=off;
	}
	else if(flag==2){
		file->f_pos=inode->ext2_inode.i_size+off;
	}
	return off;
}



int my_write(struct inode* inode,struct file* file,const char* buffer,int count){
	if(!count)
		return 0;	
	int i,j,num_of_write;
	num_of_write=file->f_pos+count;
	//printf("num_of_write %d\n",num_of_write);
	char *my_buffer=(char*)malloc(num_of_write);
	if(!my_buffer){
		perror("file_operation : fail malloc");
		exit(EXIT_FAILURE);
	}
	//my_lseek(inode,file,0,0);
	my_read(inode,file,my_buffer,file->f_pos);
	for(i=0;i<count;i++){
		my_buffer[i+file->f_pos]=buffer[i];
	}
	//printf("file_operations.c write mybuffer %.10s\n",my_buffer);
	//printf("ext2_inode.i_blocks %d\n",inode->ext2_inode.i_blocks);
	for(i=0;i<inode->ext2_inode.i_blocks;i++){
		//printf("free_block %d\n",inode->ext2_inode.i_block[i]);
		free_block(inode->ext2_inode.i_block[i]);
	}
	unsigned int temp_block;
	for(i=0,j=0;i<num_of_write;i+=BLOCK_SIZE,j++){
		temp_block=new_block();
		//printf("write block index %d\n",temp_block);
		inode->ext2_inode.i_block[j]=temp_block;
		write_block_data(temp_block,my_buffer+i);
	}
	//printf("my_buffer :%.4096s\n",my_buffer);
	free(my_buffer);
	file->f_pos=num_of_write;
	inode->ext2_inode.i_size=num_of_write;
	inode->ext2_inode.i_blocks=j;
	write_inode_data(inode->i_number,inode);
	return count;
}
/*int main(){
	struct inode inode;
	unsigned int inode_num=new_inode();
	struct file file;
	memset(&inode,'\0',sizeof(struct inode));
	memset(&file,'\0',sizeof(struct file));
	char buffer[4096*2];
	memset(buffer,'C',4096);
	char myb[1024*4];
	printf("before write f_pos:%d\n",file.f_pos);
	my_write(&inode,&file,buffer,4096);
	printf("after write f_pos:%d\n",file.f_pos);
	my_lseek(&inode,&file,0,0);
	my_read(&inode,&file,myb,4096);
	my_write(&inode,&file,buffer,4096);
	my_lseek(&inode,&file,0,0);
	my_read(&inode,&file,myb,4096);
	printf("after read f_pos:%d\n",file.f_pos);
	int count=printf("%.4096s\n",myb);
	printf("%d\n",count);
	int i;
	for(i=0;i<15;i++){
		printf("%d\n",inode.ext2_inode.i_block[i]);	
	}
	
	return 0;
}*/
