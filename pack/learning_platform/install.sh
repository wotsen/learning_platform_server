# !/bin/bash

if [ $# -lt 1 ] ; then
echo "######## no install path #########"
exit 0
fi

root_dir=$1

# 创建目录
mkdir $root_dir/learning_platform_runtime/data -p
mkdir $root_dir/learning_platform_runtime/etc -p
mkdir $root_dir/learning_platform_runtime/bin -p
mkdir $root_dir/learning_platform_runtime/log -p
mkdir $root_dir/learning_platform_runtime/err -p

# 拷贝文件
cp learning_platform_runtime/bin/* $root_dir/learning_platform_runtime/bin/ -rf
cp learning_platform_runtime/etc/* $root_dir/learning_platform_runtime/etc/ -rf
cp learning_platform_runtime/data/* $root_dir/learning_platform_runtime/data/ -rf

# 拷贝启动脚本
cp learning_platform_runtime/srv_run.sh $root_dir/learning_platform_runtime/ -f

# 特殊功能脚本

# WARNING:不允许重启计算机，除非特殊情况

# 建新启动脚本，这是解压路径的脚本
rm srv_run.sh -f
touch srv_run.sh
chmod 755 srv_run.sh

# 构建启动脚本
echo "# !/bin/bash" >> srv_run.sh
echo "cd " $root_dir/learning_platform_runtime/ >> srv_run.sh
echo "sh ./srv_run.sh &" >> srv_run.sh

# 添加到启动脚本
sudo mv srv_run.sh /etc/init.d/
cd /etc/init.d
sudo update-rc.d srv_run.sh defaults 90

# 移除启动
# sudo update-rc.d -f srv_run.sh remove
