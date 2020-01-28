#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/1/28 11:33
# @Author  : ywl
# @Email   : astralrovers@outlook.com
# @File    : setup.py.py

import sys
import os

# 不同语言安装实现不同，但是流程一致

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("use age : setup.py password zipfile")
        exit(0)

    password = sys.argv[1] # 密码
    zip_file = sys.argv[2] # 压缩包

    if not zip_file.endswith(".zip") or not os.path.exists(zip_file) or os.path.isdir(zip_file):
        print("file %s is not correct zip file." % zip_file)

    os.system("unzip -o -P %s %s" % (password, zip_file))

    cur_dir = os.getcwd()

    # FIXME:升级时可能存在某些实现会实时依赖配置文件
    install_dir = "/learning_platform/"
    os.system("mkdir %s -p" % install_dir)

    # 执行安装脚本
    os.chdir("learning_platform")
    os.system("./install.sh %s" % install_dir)

    os.chdir(cur_dir)

    # 删除解压文件
    os.system("rm learning_platform -rf")

