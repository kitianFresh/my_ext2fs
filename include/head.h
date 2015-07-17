/*各个函数的声明*/

#ifndef HEAD_H
#define HEAD_H

/*../disk_drive.c*/
/*获取磁盘block数据块，int为block的编号，指针指向数据存放的位置，返回读取的字节数*/
int get_block_data(unsigned int,void *);
/*向block中写入数据，int为block的编号，指针指向数据存放的位置，返回写入的字节数*/
int write_block_data(unsigned int,const void *);
/*取ext2_inode中的数据，index为ext2_inode的磁盘编号,放入内存i节点m_inode中*/
int get_inode_data(unsigned int index, void *m_inode);
/*向inode中写入数据，index为inode的磁盘编号，从内存i节点m_inode中读取*/
int write_inode_data(unsigned int index, void *m_inode);

/*../bitmap.c*/
/*取block中的数据，读取放到ext2_super_block结构体指针指向的空间中*/
int get_superblock_data(struct ext2_super_block * sb);
/*向磁盘写入super_block数据*/
int write_superblock_data(struct ext2_super_block * sb);
/*根据磁盘物理块号，释放该块，并置数据位图相应位置为0，成功则返回1,失败返回-1*/
int free_block(int block_num);
/*在磁盘上申请新块，并置位图数据位图相应位为1，成功返回块号,不成功返回-1*/
int new_block();
/*释放相应的inode节点,成功返回1,失败返回-1*/
int free_inode(int inode_num);
/*申请新的inode节点，成功则返回i节点号,失败返回-1*/
int new_inode();
/*获得和写回磁盘上的节点位图块，主要供以上4个函数使用,成功返回1,失败返回-1*/
int get_inodemap(char *m_inodemap,int size);
int put_inodemap(char *m_inodemap,int size);

/*获得和写回磁盘上的数据位图块，主要供以上4个函数使用,成功返回1,失败返回-1*/
int get_blockmap(char *m_blockmap,int size);
int put_blockmap(char *m_blockmap,int size);
extern unsigned int  m_inodemap[256];
extern unsigned int  m_blockmap[256];
extern struct ext2_super_block sb;



/*../namei.c*/
/*由文件名字得到文件的inode节点,如果成功解析则返回1,且相应节点数据写入m_inode中，解析失败则返回-1*/
int name_to_inode(char *path,struct inode *m_inode);



/*../inode_operations.c*/
/*在指定的目录中建立一个文件的目录项。dir指定文件建立的目录位置，name为文件名，len为文件名长度，mode指定文件的类型和访问权限。参数res_inode返回新建inode的地址*/
int create(struct inode *dir, const char *name, int len, int mode, struct inode ** res_inode);
/*目录创建函数，在dir中建立名字为name、名字长度为len，访问权限属性为mode的子目录*/
int mkdir(struct inode *dir, const char *name, int len, int mode);
/*在dir目录中删除名字为name的目录，且会递归删除其下的子目录及文件*/
int remove_dir(struct inode *dir,const char *name);
/*在dir目录中删除名字为name的文件,成功返回1,失败返回-1*/
int remove_file(struct inode *dir, const char *name);
/*在当前目录dir中打印所有的文件名字，成功返回1,失败返回-1*/
int list(struct inode *dir);




/*../file_operations.c*/
int my_read(struct inode *inode,struct file *file,char *buffer,int count);
int my_lseek(struct inode* inode,struct file* file,int off,int flag);
int my_write(struct inode* inode,struct file* file,const char* buffer,int count);

/*../namei.c*/
/*由文件名字得到文件的inode节点,如果成功解析则返回1,且相应节点数据写入m_inode中，解析失败则返回-1*/
int name_to_inode(char *path,struct inode *m_inode);

/*../mkfs_ext2.c*/
void mkfs_ext2();

/*../sys_call.c*/
extern struct task_struct task_struct;
extern struct fs_struct fs_struct;
extern struct inode root;
extern struct inode pwd;
extern struct files_struct files_struct;
extern struct file sys_file[255];
extern struct inode sys_inode[255];
extern struct file_operations file_operations;
void init();
int sys_open(char* pass,int flag);
int sys_close(int fd);
int sys_write(int fd,char *buffer,int count);
int sys_read(int fd,char *buffer,int count);
int sys_lseek(int fd,int off,int flag);
int sys_create(const char *name, int len, int mode);
int sys_mkdir(const char *name, int len, int mode);

#endif
