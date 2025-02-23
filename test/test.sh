#!/usr/bin/bash
# 创建测试目录
mkdir -p test_dir
pushd test_dir
# 创建多个文件
touch file1.txt file2.txt file3.txt
# 写入一些内容到文件中
echo "This is file1" > file1.txt
echo "This is file2" > file2.txt
echo "This is file3" > file3.txt
# 使用 cat 命令查看文件内容
cat file1.txt file2.txt file3.txt
# 使用 gzip 压缩文件
gzip file1.txt file2.txt file3.txt
# 解压缩文件
gzip -d file1.txt.gz file2.txt.gz file3.txt.gz
# 使用 zip 压缩文件
zip -m files.zip file1.txt file2.txt file3.txt
# 解压缩文件
unzip files.zip
# 使用 split 命令分割文件
split -b 10 file1.txt file1_part_
# 使用 cp 命令复制文件
cp file1.txt file1_copy.txt
# 使用 mv 命令移动文件
mv file1_copy.txt file1_moved.txt
# 使用 rm 命令删除文件
rm file1_moved.txt file2.txt file3.txt files.zip
rm file1_part_*
# 返回上级目录并删除测试目录
popd
rm -rf test_dir
echo "All tests completed."