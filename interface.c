#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"./include/file.h"
#include"./include/head.h"

void help(){
	printf("1 创建文件\n2 创建文件夹\n3 打开文件\n4 写入文件\n5 读取文件\n6 关闭文件\nrm 删除文件\ncd 改变当前工作目录\nls 显示当前文件夹的内容\nq 退出\n");
}
int main(int argc,char *argv[]){
	mkfs_ext2();
	init();
	int fd;
	int size;
	char cmd[2048];
	while(1){
		printf("my_ext2fs>>");
		scanf("%s",cmd);
		if(!strcmp(cmd,"help"))
			help();
		else if(!strcmp(cmd,"1")){
			printf("输入文件名:\n");
			scanf("%s",cmd);
			sys_create(cmd,strlen(cmd),0);
		}
		else if(!strcmp(cmd,"2")){
			printf("输入文件夹名:\n");
			scanf("%s",cmd);
			sys_mkdir(cmd,strlen(cmd),0);
		}
		else if(!strcmp(cmd,"3")){
			printf("输入文件路径:\n");
			scanf("%s",cmd);
			fd=sys_open(cmd,0);
		}
		else if(!strcmp(cmd,"4")){
			printf("输入写入数据:\n");
			scanf("%s",cmd);
			size=strlen(cmd);
			sys_lseek(fd,0,0);
			sys_write(fd,cmd,size);
		}
		else if(!strcmp(cmd,"5")){
			sys_lseek(fd,0,0);
			sys_read(fd,cmd,size);
			printf("%s\n",cmd);
		}
		else if(!strcmp(cmd,"6")){
			sys_close(fd);
		}
		else if(!strcmp(cmd,"rm")){
			printf("输入删除文件名:\n");
			scanf("%s",cmd);
			remove_file(&pwd,cmd);
		}
		else if(!strcmp(cmd,"cd")){
			printf("输入改变的目标路径:\n");
			scanf("%s",cmd);
			name_to_inode(cmd,&pwd);
		}
		else if(!strcmp(cmd,"ls")){
			list(&pwd);
		}
		else if(!strcmp(cmd,"q")){
			break;
		}
	}
	return 0;
}
