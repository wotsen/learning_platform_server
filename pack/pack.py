#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os
import json
import time
from optparse import OptionParser
from collections import namedtuple


_DEF_VERSION = "v0.0.0"

# TODO:做成交互式
# TODO:增加debug与release版本
parser = OptionParser(usage="usage: %prog [options]")
parser.add_option("-v", "--version", dest="release_version", type="string", default=_DEF_VERSION,
                  help="""
soft release version. 
将记录到release-version.json，默认值将使用最新值.
example: --verion=v1.0.0 or -v=v1.0.0""")

# action=""store是可选的，表示将会存储到options里面
parser.add_option("-b", "--build",
                  action="store", dest="build", type="int", default=4,
                  help="""
代码构建(可选1,2,3,4,):
1 --> [打包软件包], 
2 --> [make], 
3 --> [make clean], 
4 -- >[make && run], 
5 --> [make clean && make && run]
example(default): --build=4 or -b=4""")

(options, args) = parser.parse_args()


def read_last_release_version():
    """从历史读取最新版本号"""
    if not os.path.exists("release_version.json"):
        return options.release_version

    with open("release_version.json", "r", encoding="utf8") as f:
        try:
            versions = json.load(f)
            return versions["release_version"][-1][-1]
        except Exception:
            return options.release_version


def save_release_version(release_version):
    with open("release_version.json", "w+", encoding="utf8") as f:
        versions = dict()
        try:
            versions = json.load(f)
            # 如果当前版本和历史最新版本号相同则只对时间进行更新
            # TODO:如果输入的版本号小于历史版本号则报错
            if versions["release_version"][-1][-1] == release_version:
                versions["release_version"][-1][0] = time.asctime(time.localtime(time.time()))
            else:
                versions["release_versions"].append([time.asctime(time.localtime(time.time())), release_version])

        except Exception:
            versions["release_versions"] = list()
            versions["release_versions"].append([time.asctime(time.localtime(time.time())), release_version])

        json.dump(versions, f, sort_keys=True, indent=4)


def package_software(release_version):
    pass


def make_objs(release_version):
    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s" % release_version)


def clean_objs(release_version):
    os.chdir("../src/")
    os.system("make clean")


# FIXME:代码冗余，做成装饰器
def make_and_run(release_version):
    """make and run"""
    print(make_and_run.__doc__)

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s" % release_version)

    os.chdir(cur_dir)

    os.system("mkdir ai_platform/ai_runtime/bin/ -p")
    os.chdir("ai_platform/ai_runtime/bin/")
    os.system("sudo ./AIService")


def clean_make_and_run(release_version):
    """make clean and make and run"""
    print(make_and_run.__doc__)

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make clean")
    os.system("make RELEASE_VERSION=%s" % release_version)

    os.chdir(cur_dir)

    os.system("mkdir ai_platform/ai_runtime/bin/ -p")
    os.chdir("ai_platform/ai_runtime/bin/")
    os.system("sudo ./AIService")


def main():
    # 默认情况下读取历史版本号
    if options.release_version == _DEF_VERSION:
        version = read_last_release_version()

    # 记录版本号
    save_release_version(version)

    PackProc = namedtuple("PackProc", ['option', 'proc'])

    pack_procs = [
        PackProc(1, package_software),
        PackProc(2, make_objs),
        PackProc(3, clean_objs),
        PackProc(4, make_and_run),
        PackProc(5, clean_make_and_run),
    ]

    for item in pack_procs:
        if item.option == options.build:
            item.proc(version)


if __name__ == "__main__":
    print("version : ", options.release_version)
    print("build : ", options.build)
    main()
