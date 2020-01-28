#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/1/28 13:21
# @Author  : ywl
# @Email   : astralrovers@outlook.com
# @File    : pre_commit.py

import os


def must_pass_build():
    cur_dir = os.getcwd()
    os.chdir("pack/")

    # 做一次编译，编译不过不允许提交
    print("pre build..............")
    os.system("./pack.py -b1 >> .git_make")
    os.system("rm .git_make")

    os.chdir(cur_dir)

    os.system("git checkout pack/release_version.json")

    print("pre build pass..............")


if __name__ == "__main__":
    must_pass_build()
