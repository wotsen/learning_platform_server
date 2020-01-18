#!/bin/bash
install_dir=$(pwd)/../
echo "$install_dir"

# 安装libuv
cd libuv
./autogen.sh
./configure --prefix=$install_dir --enable-shared=no
make
make install
cd ..

# 安装protobuf
cd protobuf
./autogen.sh
./configure --prefix=$install_dir --enable-shared=no
make
make install
cd ..

# 安装easylogger
cd c-log
make
make MAKE_INSTALL_PREFIX=$install_dir install
cd ..

# 安装util_pthread
cd util_pthread
make
make MAKE_INSTALL_PREFIX=$install_dir install
cd ..

# 安装json
cd json
cp single_include/* $install_dir/include -rf
cd ..

# 安装SQLITE_CPP
cd SQLiteCpp
mkdir build -p
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=$install_dir
make
make install

cd sqlite3
cp libsqlite3.a $install_dir/lib/ -f
cd ../../
cp sqlite3/sqlite3.h $install_dir/include/SQLiteCpp/ -f
cd ../

mkdir ../include/sole/ -p
cp sole/sole.hpp ../include/sole/ -f

# 安装util_time
cd c-time
make
make MAKE_INSTALL_PREFIX=$install_dir install
