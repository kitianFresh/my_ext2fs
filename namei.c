#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include"./include/file.h"
#include"./include/head.h"
#define DISK "./disk"


/*由文件名字得到文件的inode节点,如果成功解析则返回1,且相应节点数据写入m_inode中，解析失败则返回-1*/
int name_to_inode(char *path,struct inode *m_inode){
	unsigned int cur_inode_num;
	char *ptoken;
	/*囧对路径,从根节点开始解析*/
	if(*path == '/'){
		ptoken = strtok(path,"/");
		cur_inode_num = 0;/*从根节点开始*/
		while(ptoken != NULL){
			//printf("%s\n",ptoken);
			if((cur_inode_num=namei(ptoken,cur_inode_num)) == -1){
				printf("no such file or dir!\n");
				return -1;
			}
			ptoken = strtok(NULL,"/");
		}
		get_inode_data(cur_inode_num,m_inode);
		return 1;
	}
	/*相对路径*/
	else if((*path == '.' & *(path+1) == '/') || (*path != '.' && *path != '/')){
		ptoken = strtok(path,"./");
		cur_inode_num = pwd.i_number;/*从当前节点开始*/
		while(ptoken != NULL){
			//printf("%s\n",ptoken);
			if((cur_inode_num=namei(ptoken,cur_inode_num)) == -1){
				printf("no such file or dir!\n");
				return -1;
			}
			ptoken = strtok(NULL,"/");
		}
		get_inode_data(cur_inode_num,m_inode);
		return 1;
	}
}
/*寻找当前节点号中是否含有名字为name的项目，如果有则返回name项目的i节点号，没有则返回-1*/
int namei(char *name, unsigned int cur_inode_num){
	struct ext2_inode *current_inode;
	if((current_inode=malloc(sizeof(struct ext2_inode))) == NULL){
		perror("namei.c: fail to malloc inode\n");
		exit(EXIT_FAILURE);
	}
	get_inode_data(cur_inode_num,current_inode);	
	
	char buf[BLOCK_SIZE];
	struct ext2_dir_entry_2 *pentry;
	int dir_datablocks = current_inode->i_blocks;/*数据块个数*/
	int i = 1;
	while(i <= dir_datablocks){
		/*取出目录节点中第i块逻辑块所在的物理块的数据，放入buf*/
		get_block_data(current_inode->i_block[i],buf);
		pentry = (struct ext2_dir_entry_2 *)buf;
		int j = 0;
		int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
		/*比较每一项*/
		while(j<num){
			/*比较pentry->inode不为0的每一项*/
			if(pentry->inode && !strcmp(&(pentry->name[0]),name)){
				return pentry->inode;
			}
			j++;
			pentry++;
		}
		i++;
	}
	free(current_inode);
	return -1;
}
