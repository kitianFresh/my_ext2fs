#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include "./include/file.h"
#include "./include/head.h"

int main(void){
	memset((char*)m_inodemap,'\0',1024);
	memset((char*)m_blockmap,'\0',1024);
	int inode_num;
	int block_num;
	int n;	
	int i;
	get_superblock_data(&sb);
	printf("super_block.s_free_blocks_count: %d\n",sb.s_free_blocks_count);
	printf("super_block.s_free_inodes_count: %d\n",sb.s_free_inodes_count);
	printf("super_block.s_wtime: %d\n",sb.s_wtime);

	get_blockmap((char*)m_blockmap,1024);
	get_inodemap((char*)m_inodemap,1024);
	for(i=0;i<256;i++){
		if(m_blockmap[i] == 0) continue;
		printf("blockmap[%d]:%#X\n",i,m_blockmap[i]);
	}
	for(i=0;i<256;i++){
		if(m_inodemap[i] == 0) continue;
		printf("inodemap[%d]:%#X\n",i,m_inodemap[i]);
	}
	
	while(1){
		printf("请输入您要选择的功能号（0-释放/1-申请/2-退出）:");
		scanf("%d",&n);
		if(n == 0){
			printf("请输入需要释放的数据块号：");
			scanf("%d",&block_num);
			if(free_block(block_num) == -1){
				perror("释放失败！\n");
				exit(EXIT_FAILURE);
			}
			get_blockmap((char*)m_blockmap,1024);
			printf("blockmap[0]:%#X\n",m_blockmap[0]);
			
			printf("请输入需要释放的i节点号：");
			scanf("%d",&inode_num);
			if(free_inode(inode_num) == -1){
				perror("释放失败！\n");
				exit(EXIT_FAILURE);
			}
			get_inodemap((char*)m_inodemap,1024);
			printf("inodemap[0]:%#X\n",m_inodemap[0]);
		}
		else if(n == 1){
			/*申请一个空闲块*/
			if((block_num=new_block()) == -1){
				perror("申请失败！\n");
				exit(EXIT_FAILURE);
			}
			printf("获得的空闲块号：%d\n",block_num);
			get_blockmap((char*)m_blockmap,1024);
			printf("blockmap[0]:%#X\n",m_blockmap[0]);
			
			/*申请一个空闲i节点*/
			if((inode_num=new_inode()) == -1){
				perror("申请失败！\n");
				exit(EXIT_FAILURE);
			}
			printf("获得的空闲i节点号：%d\n",inode_num);
			get_inodemap((char*)m_inodemap,1024);
			
			printf("inodemap[0]:%#X\n",m_inodemap[0]);
		}
		else{
			break;
		}
	}
	
	get_superblock_data(&sb);
	printf("super_block.s_free_blocks_count: %d\n",sb.s_free_blocks_count);
	printf("super_block.s_free_inodes_count: %d\n",sb.s_free_inodes_count);
	printf("super_block.s_wtime: %d\n",sb.s_wtime);
	for(i=0;i<256;i++){
		if(m_blockmap[i] == 0) continue;
		printf("blockmap[%d]:%#X\n",i,m_blockmap[i]);
	}
	for(i=0;i<256;i++){
		if(m_inodemap[i] == 0) continue;
		printf("inodemap[%d]:%#X\n",i,m_inodemap[i]);
	}
	return 0;
}

