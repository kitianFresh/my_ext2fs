#include<stdio.h>
#include<unistd.h>
#include"./include/file.h"
#include"./include/head.h"

int main(){
	//init();
	char path[]="/test";
	int fd=sys_open(path,0);
	sys_write(fd,"hello world",12);
	char buffer[20];
	sys_lseek(fd,0,0);
	sys_read(fd,buffer,12);
	printf("%s\n",buffer);
	return 0;
}
