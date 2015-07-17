#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>
#include"./include/file.h"
#include"./include/head.h"
#define DISK "./disk"

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
	//file_operations.lseek=my_seek;
	//file_operations.read=my_read;
	//file_operations.write=my_write;
	root.i_number=pwd.i_number=0;
	new_inode();
	root.ext2_inode.i_block[0]=UINT_MAX;
	root.ext2_inode.i_block[1]=new_block();
	root.ext2_inode.i_blocks=1;
	write_inode_data(0,&root);
	fs_struct.root=&root;
	fs_struct.pwd=&pwd;
}

int main(void){
	//init();
	struct ext2_dir_entry_2 *pentry;
	struct inode *res_inode;
	struct inode m_inode;
	char name[24];
	int n;
	char buf[1024];
	int i = 1;
	int num = BLOCK_SIZE/sizeof(struct ext2_dir_entry_2);
	
	while(1){
		printf("请输入您要选择的功能号\n0-创建文件\n1-创建目录\n2-输入囧对路径找相应的inode\n3-删除文件\n4-列出文件\n5-打印某一个目录下数据块内容\n6-退出:\n");
		scanf("%d",&n);
		if(n == 0){
			while(1){
				printf("请输入需要创建的文件名称（不得超过23个字符）：");
				scanf("%s",name);
				get_inode_data(0,&root);
				if(!strcmp(name,"ok")) break;
				if(create(&root,name,strlen(name),0,&res_inode) == -1){
					perror("test_inodeopt.c: create error!\n");
					exit(EXIT_FAILURE);
				}
				printf("新建文件的i节点号： %d\n",res_inode->i_number);
			}
		}
		else if(n == 1){
			while(1){
				printf("请输入需要创建的目录名称（不得超过23个字符）：");
				scanf("%s",name);
				get_inode_data(0,&root);
				if(!strcmp(name,"ok")) break;
				if(mkdir(&root,name,strlen(name),1) == -1){
					perror("test_inodeopt.c: mkdir error!\n");
					exit(EXIT_FAILURE);
				}
			}			
		}
		else if(n == 2){
			while(1){
				printf("请输入需要映射的囧对路径：");
				scanf("%s",name);
				get_inode_data(0,&root);
				if(!strcmp(name,"ok")) break;
				if(name_to_inode(name,&m_inode) == -1){
					break;
				}
				printf("inode_number: %d\n",m_inode.i_number);
			}	
		}
		else if(n == 3){
			while(1){
				printf("请输入需要删除的文件名字：");
				scanf("%s",name);
				get_inode_data(0,&root);
				if(!strcmp(name,"ok")) break;
				if(remove_file(&root,name) == -1){
					break;
				}
				printf("file %s删除成功！\n",name);
			}
		}
		else if(n == 4){
			get_inode_data(0,&root);
			list(&root);
		}
		else if(n == 5){
			while(1){
				printf("请输入需要打印数据块内容的目录名：");
				scanf("%s",name);
				if(!strcmp(name,"ok")) break;
				name_to_inode(name,&m_inode);
				printf(".ext2_inode.i_blocks: %d\n",m_inode.ext2_inode.i_blocks);
				printf("inode    \trec_len  \tname_len \tfile_type\tname\n");
				i = 1;
				while(i <= m_inode.ext2_inode.i_blocks){
					get_block_data(m_inode.ext2_inode.i_block[i],buf);
					pentry = (struct ext2_dir_entry_2*)buf;
					int j = 0;
					for(;j<num;j++,pentry++){
						printf("%-9d\t",pentry->inode);
						printf("%-9d\t",pentry->rec_len);
						printf("%-9d\t",pentry->name_len);
						printf("%-9X\t",pentry->file_type);
						printf("%-24s\n",pentry->name);	
					}
					i++;
				}
				i = 0;
				while(i <= m_inode.ext2_inode.i_blocks){
					printf(".ext2_inode.i_block[%d]: %d\n",i,m_inode.ext2_inode.i_block[i]);
					i++;
				}
			}
		}
		else {
			break;
		}
	}
	
	return 0;
}
