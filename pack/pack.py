#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os
import json
import time
import re
from optparse import OptionParser
from collections import namedtuple


_DEF_VERSION = "v0.0.0"

# TODO:做成交互式
parser = OptionParser(usage="usage: %prog [options]")

parser.add_option("-d", "--debug", dest="debug", action="store_true", default=False,
                    help="debug模式,默认非debug模式,如果是debug版本：-d or --debug")

parser.add_option("-v", "--version", dest="release_version", type="string", default=_DEF_VERSION,
                  help="""
soft release version. 
将记录到release-version.json，默认值将使用最新值.
example: --verion=v1.0.0 or -vv1.0.0""")

parser.add_option("-b", "--build",
                  action="store", dest="build", type="int", default=4,
                  help="""
代码构建(可选1,2,3,4,):
1 --> [打包软件包], 
2 --> [make], 
3 --> [make clean], 
4 -- >[make && run], 
5 --> [make clean && make && run]
example(default): --build=4 or -b4""")

(options, args) = parser.parse_args()


def read_last_release_version():
    """从历史读取最新版本号"""

    print("读取历史版本号")

    if not os.path.exists("release_version.json"):
        return options.release_version

    with open("release_version.json", "r", encoding="utf8") as f:
        try:
            versions = json.load(f)
            return versions["release_versions"][-1][-1]
        except Exception:
            # 异常时，使用输入的版本号
            return options.release_version


def save_release_version(release_version):
    # 记录历史版本号

    with open("release_version.json", "r+", encoding="utf8") as f:
        versions = dict()
        try:
            versions = json.load(f)
            # 如果当前版本和历史最新版本号相同则只对时间进行更新
            if versions["release_versions"][-1][-1] == release_version:
                versions["release_versions"][-1][0] = time.asctime(time.localtime(time.time()))
            else:
                versions["release_versions"].append([time.asctime(time.localtime(time.time())), release_version])

        except Exception as e:
            print("解析版本记录失败 : ", e)
            print("重置记录处理，请确认历史版本正确记录")
            versions["release_versions"] = list()
            versions["release_versions"].append([time.asctime(time.localtime(time.time())), release_version])

        f.seek(0)
        f.truncate()
        json.dump(versions, f, sort_keys=True, indent=4)


def check_build_version(old_version, new_version):
    print("检查版本号")

    if not re.match(r'[vV](0|[1-9][0-9]*.?)\.(0|[1-9][0-9]*.?)\.(0|[1-9][0-9]*.?)$', new_version):
        print("%s 版本号格式错误，v不区分大小写，以0开头的版本好只能有一个数字." % new_version)
        exit(-1)

    if old_version.lower() > new_version.lower():
        print("新版本号错误，不能比历史最近版本号还小，old:%s, new:%s" % (old_version, new_version))
        exit(-1)


def package_software(release_version, debug):
    pass


def make_objs(release_version, debug):
    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s" % release_version)


def clean_objs(release_version, debug):
    os.chdir("../src/")
    os.system("make clean")


# FIXME:代码冗余，做成装饰器
def make_and_run(release_version, debug):
    """make and run"""
    print(make_and_run.__doc__)

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, "ndebug=true" if not debug else ""))

    os.chdir(cur_dir)

    os.system("mkdir ai_platform/ai_runtime/bin/ -p")
    os.chdir("ai_platform/ai_runtime/bin/")
    os.system("sudo ./AIService")


def clean_make_and_run(release_version, debug):
    """make clean and make and run"""
    print(make_and_run.__doc__)

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make clean")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, "ndebug=true" if not debug else ""))

    os.chdir(cur_dir)

    os.system("mkdir ai_platform/ai_runtime/bin/ -p")
    os.chdir("ai_platform/ai_runtime/bin/")
    os.system("sudo ./AIService")


def main():
    # 读取历史版本号
    version = read_last_release_version()

    # 校验版本号
    check_build_version(version, options.release_version)
    
    # 默认情况下使用版本号
    if options.release_version.lower() != _DEF_VERSION:
        version = options.release_version.lower()

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
            item.proc(version, options.debug)
            return
    
    print("不支持构建选项: ", options.build)
    print("使用 --help 查看帮助")


if __name__ == "__main__":
    print("version : ", options.release_version)
    print("build : ", options.build)
    print("debug : ", options.debug)
    main()
