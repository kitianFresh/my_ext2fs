#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "./include/file.h"
#include "./include/head.h"


struct task_struct task_struct;
struct fs_struct fs_struct;
struct inode root;
struct inode pwd;
struct files_struct files_struct;
struct file sys_file[255];//系统打开文件表
struct inode sys_inode[255];//系统打开inode表
struct file_operations file_operations;

void init(){
	memset(&task_struct,'\0',sizeof(struct task_struct));
	memset(&fs_struct,'\0',sizeof(struct fs_struct));
	memset(&root,'\0',sizeof(struct inode));
	memset(&pwd,'\0',sizeof(struct inode));
	memset(&files_struct,'\0',sizeof(struct files_struct));
	memset(sys_file,'\0',sizeof(struct file)*255);
	memset(sys_inode,'\0',sizeof(struct inode)*255);
	memset(&file_operations,'\0',sizeof(struct file_operations));
	task_struct.fs=&fs_struct;
	task_struct.files=&files_struct;
	file_operations.lseek=my_lseek;
	file_operations.read=my_read;
	file_operations.write=my_write;
	root.i_number=pwd.i_number=0;
	new_inode();
	root.ext2_inode.i_block[0]=UINT_MAX;
	root.ext2_inode.i_block[1]=new_block();
	root.ext2_inode.i_blocks=1;
	pwd.ext2_inode.i_block[0]=root.ext2_inode.i_block[0];
	pwd.ext2_inode.i_block[1]=root.ext2_inode.i_block[1];
	pwd.ext2_inode.i_blocks=1;
	write_inode_data(0,&root);
	fs_struct.root=&root;
	fs_struct.pwd=&pwd;
}
int get_free_sys_file(){
	int i;
	for(i=0;i<255;i++){
		if(sys_file[i].f_inode==NULL)
			return i;
	}
	return -1;
}
int get_free_sys_inode(){
	int i;
	for(i=0;i<255;i++){
		if(sys_inode[i].i_number==0)
			return i;
	}
	return -1;
}
int get_free_user_file(){
	int i;
	for(i=0;i<255;i++){
		if(files_struct.fd[i]==NULL)
			return i;
	}
	return -1;
}
int sys_open(char* pass,int flag){
	int free_sys_inode;
   	int free_sys_file;
	int free_user_file;
	int i;	
	free_sys_inode=get_free_sys_inode();
	if(free_sys_inode==-1){
		perror("sys_inode[] full");
		return -1;
	}
	name_to_inode(pass,sys_inode+free_sys_inode);
	
	//如果该inode存在，则只保留一个
	for(i=0;i<free_sys_inode;i++){
		if(sys_inode[i].i_number==sys_inode[free_sys_inode].i_number){
			sys_inode[free_sys_inode].i_number=0;
			free_sys_inode=i;		
		}
	}
	free_sys_file=get_free_sys_file();
	if(free_sys_file==-1){
		perror("sys_file[] full");
		return -1;
	}
	sys_file[free_sys_file].f_inode=sys_inode+free_sys_inode;
	sys_file[free_sys_file].f_op=&file_operations;
	free_user_file=get_free_user_file();
	if(free_user_file==-1){
		perror("user file full");
		return -1;
	}
	files_struct.fd[free_user_file]=sys_file+free_sys_file;
	return free_user_file;
}

int sys_close(int fd){
	files_struct.fd[fd]->f_inode->i_number=0;
	files_struct.fd[fd]->f_inode=NULL;
	files_struct.fd[fd]=NULL;
	return 0;
}

int sys_write(int fd,char *buffer,int count){
	return my_write(files_struct.fd[fd]->f_inode,files_struct.fd[fd],buffer,count);
}

int sys_read(int fd,char *buffer,int count){
	return my_read(files_struct.fd[fd]->f_inode,files_struct.fd[fd],buffer,count);	
}

int sys_lseek(int fd,int off,int flag){
	return my_lseek(files_struct.fd[fd]->f_inode,files_struct.fd[fd],off,flag);
}

int sys_create(const char *name, int len, int mode){
	int free_sys_inode;
   	int free_sys_file;
	int free_user_file;
	int i;	
	free_sys_inode=get_free_sys_inode();
	if(free_sys_inode==-1){
		perror("sys_inode[] full");
		return -1;
	}
	struct inode* temp_inode;
	temp_inode=sys_inode+free_sys_inode;
	create(&pwd,name,len,mode,&temp_inode);
	//如果该inode存在，则只保留一个
	for(i=0;i<free_sys_inode;i++){
		if(sys_inode[i].i_number==sys_inode[free_sys_inode].i_number){
			sys_inode[free_sys_inode].i_number=0;
			free_sys_inode=i;		
		}
	}
	free_sys_file=get_free_sys_file();
	if(free_sys_file==-1){
		perror("sys_file[] full");
		return -1;
	}
	sys_file[free_sys_file].f_inode=sys_inode+free_sys_inode;
	sys_file[free_sys_file].f_op=&file_operations;
	free_user_file=get_free_user_file();
	if(free_user_file==-1){
		perror("user file full");
		return -1;
	}
	files_struct.fd[free_user_file]=sys_file+free_sys_file;
	return free_user_file;
}

int sys_mkdir(const char *name, int len, int mode){
	return mkdir(&pwd,name,len,mode);
}

/*int main(){
	//init();
	char path[]="/test";
	int fd=sys_open(path,0);
	sys_write(fd,"hello world",12);
	char buffer[20];
	sys_lseek(fd,0,0);
	sys_read(fd,buffer,12);
	printf("%s\n",buffer);
	return 0;
}*/
