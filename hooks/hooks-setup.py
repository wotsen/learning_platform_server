#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/1/28 13:36
# @Author  : ywl
# @Email   : astralrovers@outlook.com
# @File    : hooks-setup.py.py

# 用于一键部署hooks

import os


def git_hooks():
    """
    git的钩子脚本
    """
    os.system("chmod +x ./ -R")
    os.system("cp pre-commit.py ../.git/hooks/pre-commit")
    os.system("cp commit-msg.py ../.git/hooks/commit-msg")


if __name__ == "__main__":
    git_hooks()
