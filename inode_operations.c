#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>
#include"./include/file.h"
#include"./include/head.h"
#define DISK "./disk"

/*check(dir,name)文件搜索函数，在dir目录中搜索名字为name的文件存在返回值1,不存在该文件，返值为-1*/
int is_exist(struct inode *dir ,const char *name);
/*检验名字的合法性，合法返回1,不合法返回-1*/
int checkname(const char *name);

int checkname(const char *name){
	const char *p;
	p = name;
	while(*p !='\0'){
		if(*p == '/' || *p == '%' || *p == '@') return -1;
		p++;
	}
	return 1;
}

int is_exist(struct inode *dir, const char *name){
	/*判断目录是否有效*/
	if(dir->ext2_inode.i_block[0] != UINT_MAX){
		perror("inode_operations.c: check error! dir is a file!\n");
		return -1;
	}
	char buf[BLOCK_SIZE];
	struct ext2_dir_entry_2 *pentry;
	int dir_datablocks = dir->ext2_inode.i_blocks;/*数据块个数*/
	int i = 1;
	int j = 0;
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	while(i <= dir_datablocks){
		/*取出目录节点中第i块逻辑块所在的物理块的数据，放入buf*/
		get_block_data(dir->ext2_inode.i_block[i],buf);
		pentry = (struct ext2_dir_entry_2 *)buf;
		//printf("inode_operations.c: check : dir_datablocks: %d\n",i);
		/*比较每一项*/
		while(j<num){
			//printf("inode_operations.c: check : entry: %d\n",j);
			/*比较pentry->inode不为0的每一项*/
			if(pentry->inode && !strcmp(pentry->name,name)){
				return 1;
			}
			j++;
			pentry++;
		}
		i++;
	}
	return -1;
}

int create(struct inode *dir, const char *name, int len, int mode, struct inode ** res_inode){
	/*判断目录是否有效*/
	if(dir->ext2_inode.i_block[0] != UINT_MAX){
		perror("inode_operations.c: create error! dir is a file!\n");
		return -1;
	}
	/*判断名字是否有效*/
	if(checkname(name) == -1){
		perror("inode_operations.c: create error! filename is illegal!\n");
		return -1;
	}
	/*判断是否已存在*/
	if(is_exist(dir,name) == 1){
		perror("inode_operations.c: create error! file is alreay exists!\n");
		return -1;
	}
	char buf[BLOCK_SIZE];
	struct ext2_dir_entry_2 *pentry;
	int dir_datablocks = dir->ext2_inode.i_blocks;/*数据块个数*/
	int i = 1;
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	while(i <= dir_datablocks){
		/*取出目录节点中第i块逻辑块所在的物理块的数据，放入buf*/
		get_block_data(dir->ext2_inode.i_block[i],buf);
		pentry = (struct ext2_dir_entry_2 *)buf;
		/*寻找pentry->inode为0的每一项，表示未使用,填写目录项*/
		//printf("inode_operations.c: create : dir_datablocks: %d\n",i);
		int j = 0;
		while(j<num){
			//printf("inode_operations.c: create : entry: %d\n",j);
			if(!pentry->inode){
				pentry->name_len = len;
				strcpy(pentry->name,name);
				pentry->file_type = mode;
				pentry->rec_len = 8 + len;
				pentry->inode = new_inode();
				write_block_data(dir->ext2_inode.i_block[i],buf);//更新该数据块
				*res_inode=(struct inode *)malloc(sizeof(struct inode));
				if(*res_inode != NULL){
					get_inode_data(pentry->inode,*res_inode);
					return 1;
				}
			}
			j++;
			pentry++;
		}
		i++;
	}
	/*数据块表项已满，则需申请新的数据块来存放新的目录项*/
	if(dir_datablocks>=14){
		perror("inode_operations.c create error! dir_entry is full,no more sub_dir!\n");
		return -1;
	}
	dir_datablocks = ++(dir->ext2_inode.i_blocks);
	dir->ext2_inode.i_block[dir_datablocks] = new_block();
	write_inode_data(dir->i_number,dir);//因为申请了新块，inode节点及时更新

	get_block_data(dir->ext2_inode.i_block[dir_datablocks],buf);
	pentry = (struct ext2_dir_entry_2 *)buf;
	pentry->name_len = len;
	strcpy(pentry->name,name);
	pentry->file_type = mode;
	pentry->rec_len = 8 + len;
	pentry->inode = new_inode();
	write_block_data(dir->ext2_inode.i_block[dir_datablocks],buf);//更新该数据块
	if((*res_inode=(struct inode *)malloc(sizeof(struct inode))) != NULL){
		get_inode_data(pentry->inode,*res_inode);
		return 1;
	}
	return -1;
}

int mkdir(struct inode *dir, const char *name, int len, int mode){
	/*判断目录是否有效*/
	if(dir->ext2_inode.i_block[0] != UINT_MAX){
		perror("inode_operations.c: mkdir error! dir is a file!\n");
		return -1;
	}
	/*判断名字是否有效*/
	if(checkname(name) == -1){
		perror("inode_operations.c: mkdir error! dirname is illegal!\n");
		return -1;
	}
	/*判断是否已存在*/
	if(is_exist(dir,name) == 1){
		perror("inode_operations.c: mkdir error! dirname is alreay exists!\n");
		return -1;
	}
	char buf[BLOCK_SIZE];
	struct inode *m_inode;
	struct ext2_dir_entry_2 *pentry;
	int dir_datablocks = dir->ext2_inode.i_blocks;/*数据块个数*/
	int i = 1;
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	while(i <= dir_datablocks){
		/*取出目录节点中第i块逻辑块所在的物理块的数据，放入buf*/
		get_block_data(dir->ext2_inode.i_block[i],buf);
		pentry = (struct ext2_dir_entry_2 *)buf;
		
		/*寻找pentry->inode为0的每一项，表示未使用*/
		int j = 0;
		while(j<num){
			if(!pentry->inode){
				pentry->name_len = len;
				strcpy(pentry->name,name);
				pentry->file_type = mode;
				pentry->rec_len = 8 + len;
				pentry->inode = new_inode();
				write_block_data(dir->ext2_inode.i_block[i],buf);//更新该数据块
				/*下面更新为目录项（文件夹）申请的inode节点信息，并为其预分配一块数据块，用于存放子目录或文件*/
				if((m_inode=(struct inode*)malloc(sizeof(struct inode))) != NULL){
					get_inode_data(pentry->inode,m_inode);
					m_inode->ext2_inode.i_block[0] = UINT_MAX;//表示该节点是目录节点
					m_inode->ext2_inode.i_block[1] = new_block();//为每个目录节点预分配一块数据块，存放目录项
					m_inode->ext2_inode.i_blocks = 1;		
					write_inode_data(pentry->inode,m_inode);
					free(m_inode);
					return 1;
				}
			}
			j++;
			pentry++;
		}
		i++;
	}
	/*数据块表项已满，则需申请新的数据块来存放新的目录项*/
	if(dir_datablocks>=14){
		perror("inode_operations.c mkdir error! dir_entry is full,no more sub_dir!\n");
		return -1;
	}
	dir_datablocks = ++dir->ext2_inode.i_blocks;
	dir->ext2_inode.i_block[dir_datablocks] = new_block();
	write_inode_data(dir->i_number,dir);//因为申请了新块，inode节点及时更新

	get_block_data(dir->ext2_inode.i_block[dir_datablocks],buf);
	pentry = (struct ext2_dir_entry_2 *)buf;
	pentry->name_len = len;
	strcpy(pentry->name,name);
	pentry->file_type = mode;
	pentry->rec_len = 8 + len;
	pentry->inode = new_inode();
	write_block_data(dir->ext2_inode.i_block[dir_datablocks],buf);//更新该数据块
	
	if((m_inode=(struct inode*)malloc(sizeof(struct inode))) != NULL){
		get_inode_data(pentry->inode,m_inode);
		m_inode->ext2_inode.i_block[0] = UINT_MAX;//表示该节点是目录节点
		m_inode->ext2_inode.i_block[1] = new_block();//为每个目录节点预分配一块数据块，存放目录项
		m_inode->ext2_inode.i_blocks = 1;		
		write_inode_data(pentry->inode,m_inode);
		free(m_inode);
		return 1;
	}

	return -1;
}


int list(struct inode *dir){
	if(dir->ext2_inode.i_block[0] != UINT_MAX){
		perror("dir is a file!\n");
		return -1;
	} 
	struct ext2_dir_entry_2 *pentry;
	char buf[1024];
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	int i = 1;
	while(i <= dir->ext2_inode.i_blocks){
		get_block_data(dir->ext2_inode.i_block[i],buf);
		pentry = (struct ext2_dir_entry_2*)buf;
		int j = 0;
		for(;j<num;j++,pentry++){
			if(pentry->inode == 0) continue;
			printf("%-24s\n",pentry->name);	
		}
		i++;
	}
	return 1;
}

int remove_file(struct inode *dir, const char *name){
	/*判断目录是否有效*/
	if(dir->ext2_inode.i_block[0] != UINT_MAX){
		perror("inode_operations.c: remove_file error! current dir is a file!\n");
		return -1;
	}
	/*判断名字是否有效*/
	if(checkname(name) == -1){
		perror("inode_operations.c: remove_file error! filename is illegal!\n");
		return -1;
	}
	char buf[BLOCK_SIZE];
	struct ext2_dir_entry_2 *pentry;
	int dir_datablocks = dir->ext2_inode.i_blocks;/*数据块个数*/
	int i = 1;
	int j = 0;
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	while(i <= dir_datablocks){
		/*取出目录节点中第i块逻辑块所在的物理块的数据，放入buf*/
		get_block_data(dir->ext2_inode.i_block[i],buf);
		pentry = (struct ext2_dir_entry_2 *)buf;
		printf("inode_operations.c: check : dir_datablocks: %d\n",i);
		/*比较每一项*/
		while(j<num){
			printf("inode_operations.c: check : entry: %d\n",j);
			/*比较pentry->inode不为0的每一项*/
			if(pentry->inode && !strcmp(pentry->name,name)){
				if(free_inode(pentry->inode) == -1){
					perror("节点释放失败!\n");
					return -1;
				};
				/*清空该目录项*/
				memset(pentry,'\0',sizeof(struct ext2_dir_entry_2));
				/*写回磁盘，更新数据*/
				write_block_data(dir->ext2_inode.i_block[i],buf);
				return 1;
			}
			j++;
			pentry++;
		}
		i++;
	}
	printf("no such file,please check your filename!\n");
	return -1;
}

