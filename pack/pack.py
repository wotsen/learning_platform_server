#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os
import json
import time
import re
import _thread
import zipfile
from optparse import OptionParser
from collections import namedtuple

_DEF_VERSION = "0.0.0"

parser = OptionParser(usage="usage: %prog [options]")

parser.add_option("-d", "--debug", dest="debug", action="store_true", default=False,
                  help="debug模式,默认非debug模式,如果是debug版本：-d or --debug")

parser.add_option("-v", "--version", dest="release_version", type="string", default=_DEF_VERSION,
                  help="""
soft release version. 
将记录到release-version.json，默认值将使用最新值.
example: --version=1.0.0 or -v1.0.0""")

parser.add_option("-p", "--patch", dest="patch", action="store_true", default=False,
                  help="全量包还是增量包，默认全量包，如果是增量包：-d or --debug")

parser.add_option("-n", "--name", dest="name", type="int", default=0,
                  help="""
构建升级包，可按需要进行选择，目的是支持全量和增量:
1 --> 构建所有升级包
2 --> 只构建打包服务器(不支持)
3 --> 只构建打包system cmd代理服务器(不支持)
""")

parser.add_option("-b", "--build",
                  action="store", dest="build", type="int", default=4,
                  help="""
工程构建，将会构建所有服务器所需要的资源(可选1,2,3,4):
1 --> [make], 
2 --> [make clean], 
3 --> [make clean && make], 
4 --> [make && run], 
5 --> [make clean && make && run]
example(default): --build=4 or -b4""")

(options, args) = parser.parse_args()

crc_list = (
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
)


def sum_crc16(crc, file_bit):
    """
计算CRC16
    @param crc:初始校验码
    @param file_bit:文件2进制流
    @return:校验码
    """
    for bit in file_bit:
        crc = 0xffff & crc
        temp = crc // 256
        crc = 0xffff & crc
        crc <<= 8
        crc = 0xffff & crc
        crc ^= crc_list[0xff & (temp ^ bit)]
    return crc


def sum_file_crc16(file_name):
    """
计算文件校验码，每次计算4096字节
    @param file_name:文件名
    @return:校验码
    """
    crc = 0
    with open(file_name, 'rb') as f:
        while True:
            line = f.read(4096)
            if not line:
                break
            else:
                crc = sum_crc16(crc, line)
    return crc


def read_last_release_version():
    """从历史读取最新版本号
    @return: 历史最新的版本号
    """

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
    """
记录历史版本号
    @param release_version:输入版本号
    """

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
    """
检查版本号
    @param old_version:旧版本号
    @param new_version:新版本号
    """
    print("检查版本号")

    if not re.match(r'(0|[1-9][0-9]*.?)\.(0|[1-9][0-9]*.?)\.(0|[1-9][0-9]*.?)$', new_version):
        print("%s 版本号格式错误，以0开头的版本号只能有一个数字." % new_version)
        exit(-1)

    if old_version > new_version:
        print("新版本号错误，不能比历史最近版本号还小，old:%s, new:%s" % (old_version, new_version))
        exit(-1)


_SRV_DEBUG = "ndebug=true"


def srv_debug(debug):
    """
调试版本的编译选项
    @param debug:调试使能
    @return:编译选项
    """
    if debug:
        return ""
    else:
        return _SRV_DEBUG


def make_objs(release_version, debug):
    """
只进行编译
    @param release_version: 版本号
    @param debug: 调试使能
    """
    print("只进行编译")
    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, srv_debug(debug)))


def clean_objs(release_version, debug):
    """
清除编译产物
    @param release_version:版本号
    @param debug:调试使能
    """

    print("清除编译产物")

    os.chdir("../src/")
    os.system("make clean")


def clean_make_objs(release_version, debug):
    """
清除后编译
    @param release_version:版本号
    @param debug:调试使能
    """
    print("清除后编译")

    cur_dir = os.getcwd()
    os.chdir("../src/")
    os.system("make clean")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, srv_debug(debug)))
    os.chdir(cur_dir)


def run_system_cmd_proxyd():
    """
启动systemcmd代理"
    """
    os.system("./systemcmd_proxyd")


def make_and_run(release_version, debug):
    """
编译并且运行
    @param release_version:版本号
    @param debug:调试使能
    """
    print("编译并且运行")

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, srv_debug(debug)))

    os.chdir(cur_dir)

    os.system("mkdir learning_platform/learning_platform_runtime/bin/ -p")
    os.system("cp ../src/AIService learning_platform/learning_platform_runtime/bin/")
    os.system("cp ../bin/systemcmd_proxyd learning_platform/learning_platform_runtime/bin/")
    os.chdir("learning_platform/learning_platform_runtime/bin/")
    _thread.start_new_thread(run_system_cmd_proxyd, ())
    os.system("sudo ./AIService")


def clean_make_and_run(release_version, debug):
    """
重新编译，然后运行
    @param release_version:版本号
    @param debug:调试使能
    """
    print("重新编译，然后运行")

    cur_dir = os.getcwd()

    os.chdir("../src/")
    os.system("make clean")
    os.system("make RELEASE_VERSION=%s %s" % (release_version, srv_debug(debug)))

    os.chdir(cur_dir)

    os.system("mkdir learning_platform/learning_platform_runtime/bin/ -p")

    # 程序拷贝
    os.system("cp ../src/AIService learning_platform/learning_platform_runtime/bin/")
    os.system("cp ../bin/systemcmd_proxyd learning_platform/learning_platform_runtime/bin/")

    os.chdir("learning_platform/learning_platform_runtime/bin/")

    # 启动systemcmd 代理服务器
    _thread.start_new_thread(run_system_cmd_proxyd, ())

    # 启动应用
    os.system("sudo ./AIService")


def build_proc(build_option, version, debug):
    """
构建
    @param build_option:修建选项，选择构建方式
    @param version:版本
    @param debug:调试使能
    @return:
    """
    BuildProc = namedtuple("BuildProc", ['option', 'proc'])

    build_proc = (
        BuildProc(1, make_objs),
        BuildProc(2, clean_objs),
        BuildProc(3, clean_make_objs),
        BuildProc(4, make_and_run),
        BuildProc(5, clean_make_and_run),
    )

    for item in build_proc:
        if item.option == build_option:
            item.proc(version, debug)
            return

    print("不支持构建选项: ", build_option)
    print("使用 --help 查看帮助")


def get_zip_file(input_path, result):
    """
    对目录进行深度优先遍历
    @param input_path:路径
    @param result:结果
    @return:
    """
    files = os.listdir(input_path)

    if not files:
        result.append(input_path)
        return

    for file in files:
        if os.path.isdir(input_path + '/' + file):
            get_zip_file(input_path + '/' + file, result)
        else:
            result.append(input_path + '/' + file)


_PATCH = "patch"
_UPDATE = "update"
_DEBUG = "d"
_RELEASE = "r"


def get_zip_name(pack_type, pack_name, version, debug):
    """
获取zip包名称
    @param pack_type:打包类型
    @param pack_name:打包名称
    @param version:版本
    @param debug:调试使能
    @return:
    """
    return "%s_%s_package_%s%s_%s" % \
           (_PATCH if pack_type else _UPDATE,
            pack_name,
            time.strftime("%Y-%m-%d_%H-%M-%S", time.localtime()),
            _DEBUG if debug else _RELEASE,
            version)


def zip_dir_file(file_dir, start_dir, out_dir, zip_name, password=None):
    """
使用zip压缩文件
    @param file_dir: 文件所在目录
    @param start_dir: 起始目录
    @param out_dir: 输出目录
    @param zip_name: 压缩名称
    @param password: 密码，为空时不加密
    @return:
    """
    cur_dir = os.getcwd()
    os.chdir(file_dir)

    if password:
        pass_str = "-P " + password
    else:
        pass_str = " "

    print("开始压缩")

    # 压缩文件
    os.system("zip -r %s %s/%s %s/*" % (pass_str, out_dir, zip_name, start_dir))
    os.chdir(cur_dir)

    print("计算crc16")

    # 校验CRC
    crc = hex(sum_file_crc16(zip_name)).upper()[2:]
    crc = '0' * (4 - len(crc)) + crc

    # 添加crc后缀
    last_name = "%s_crc%s.zip" % (zip_name, crc)

    # 重命名
    os.system("mv %s %s" % (zip_name, last_name))

    # 删除中间文件
    os.system("rm %s -rf" % file_dir)

    return last_name


def pack_learn_platform_server(pack_type, pack_name, version, debug):
    """
打包压缩服务器发布包
    @param pack_type:打包类型，update全量还是patch增量
    @param pack_name:名称
    @param version:版本
    @param debug:调试使能
    """
    # make编译
    # clean_make_objs(version, debug)

    print("编译结束，开始打包 %s" % pack_name)

    # 清除文件
    os.system("rm zip_dir -rf")
    os.system("rm *.zip -f")

    print("创建目录")

    # 创建目录
    os.system("mkdir zip_dir/learning_platform/learning_platform_runtime/bin -p")
    os.system("mkdir zip_dir/learning_platform/learning_platform_runtime/etc -p")
    os.system("mkdir zip_dir/learning_platform/learning_platform_runtime/data -p")
    os.system("mkdir zip_dir/learning_platform/learning_platform_runtime/log -p")
    os.system("mkdir zip_dir/learning_platform/learning_platform_runtime/err -p")

    print("程序拷贝")

    # 程序拷贝
    os.system("cp ../src/AIService zip_dir/learning_platform/learning_platform_runtime/bin/")
    os.system("cp ../bin/systemcmd_proxyd zip_dir/learning_platform/learning_platform_runtime/bin/")

    print("配置文件拷贝")

    # 配置文件拷贝
    os.system("cp learning_platform/learning_platform_runtime/etc/*.json zip_dir/learning_platform/learning_platform_runtime/etc/")

    print("默认参数拷贝")

    # 默认参数拷贝
    os.system("cp learning_platform/learning_platform_runtime/data/sys_default_param.json zip_dir/learning_platform/learning_platform_runtime/data/")

    print("安装脚本拷贝")

    # 安装脚本拷贝
    os.system("cp learning_platform/install.sh zip_dir/learning_platform/ -r")

    print("启动脚本拷贝")

    # 启动脚本拷贝
    os.system("cp learning_platform/learning_platform_runtime/srv_run.sh zip_dir/learning_platform/learning_platform_runtime/srv_run.sh")

    # 时间戳
    zip_name = get_zip_name(pack_type, pack_name, version, debug)

    print("压缩文件")

    last_name = zip_dir_file("zip_dir", "learning_platform", "../", zip_name, "pass")

    print("压缩完成 ", last_name)

    # 存放压缩包
    os.system("cp %s packages/" % last_name)

    print("打包结束")


PackProc = namedtuple("PackProc", ['option', "name", 'proc'])

pack_proc = (
    PackProc(1, "learn_platform_server", pack_learn_platform_server),
)


def package_software(pack_type, name_idx, version, debug):
    """
打包压缩软件
    @param pack_type: 打包类型，update全量还是patch增量
    @param name_idx: 名称索引
    @param version: 版本
    @param debug: 调试使能
    @return:
    """
    for item in pack_proc:
        if item.option == name_idx:
            item.proc(pack_type, item.name, version, debug)
            return

    print("不支持打包选项: ", name_idx)
    print("使用 --help 查看帮助")


def main():
    # 读取历史版本号
    version = read_last_release_version()

    # 默认情况下使用版本号
    if options.release_version != _DEF_VERSION:
        # 校验版本号
        check_build_version(version, options.release_version)
        version = options.release_version

    # 记录版本号
    save_release_version(version)

    # TODO:打印选项，增加更多提示信息
    print("version : ", version)
    print("build : ", options.build)
    print("debug : ", options.debug)

    # 添加前缀
    version = 'v' + version

    if options.name != 0:
        print("update pack : ", not options.patch)
        print("pack name : ", options.name)
        package_software(options.patch, options.name, version, options.debug)
        return

    build_proc(options.build, version, options.debug)


if __name__ == "__main__":
    main()
