仿真一个ext2文件系统。

建立一个9220KB的文件在当前目录下，用这个文件模拟磁盘。
dd if=/dev/zero of=./disk bs=1K count=9220

数据结构的大小：
block : 1KB
inode : 128B

整个磁盘的划分为：
引导块 : 1KB
超级块 : 1KB
inode bitmap : 1KB
block bitmap : 1KB
inode : 1M (共8*1024个inode)
block : 8M (共8*1024个block)

添加了 mkfs_ext2 文件夹，里面的 mkfs_ext2.c 用来对 disk文件进行初始化，
由于在open中使用了 O_CREAT标志，所以上面用dd命令建立disk文件的步骤可以省略。

在 include/file.h 中添加了 其他结构体。暂时不使用的结构体用空结构体补全。
# my_ext2fs
一个ext2文件系统仿真（a ext2 filesystem simulation）
