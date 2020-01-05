#!/bin/bash
install_dir=$(pwd)/../
echo "$install_dir"

# 安装easylogger
cd c-log
make
make MAKE_INSTALL_PREFIX=$install_dir install

# 安装util_pthread
cd ../util_pthread
make
make MAKE_INSTALL_PREFIX=$install_dir install

# 安装json
cd ../json
cp single_include/* $install_dir/include -rf

# 安装libuv
cd ../libuv
./autogen.sh
./configure --prefix=$install_dir
make
make install

# 安装protobuf
cd ../protobuf
./autogen.sh
./configure --prefix=$install_dir
make
make install

# 安装SQLITE_CPP
cd ../SQLiteCpp
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$install_dir ..
make
make install

cd sqlite3
make install

cd ../../

mkdir ../include/sole/ -p
cp sole/sole.hpp ../include/sole/ -f

# 安装util_time
cd util_time
make
make MAKE_INSTALL_PREFIX=$install_dir install
