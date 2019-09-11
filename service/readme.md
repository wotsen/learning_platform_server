<!--
 * @Date: 2019-08-10 16:11:54
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-24 21:28:14
 -->
# 说明

`service`为客户端、web提供接口；调度深度学习；另外提供其他的服务。

## 目录说明

- `app`为项目业务代码路径
- `includes`为外部依赖库的头文件
- `libs`为外部依赖库
- `libs_src`为外部依赖库修改过的源码，用于修改该库时重新编译为库文件
- `test`为测试代码

## 外部依赖

分为项目自带库和系统自带库。

[参考](https://github.com/programthink/opensource/blob/master/libs/cpp.wiki#21)

### 需要操作系统安装的库

- `boost`库，覆盖`c++`很全的库
- https://pocoproject.org/docs/package-Foundation.Filesystem.html  和`boost`一样很全面
- `openssl`库

### 项目自带的外部开源依赖库

见`libs_src`