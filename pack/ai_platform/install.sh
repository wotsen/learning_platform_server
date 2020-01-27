# !/bin/bash
root_dir="~/.ai_platform"
# 基本文件夹

if [ ! -d "$root_dir" ]; then
    mkdir $root_dir

if [ ! -d "$root_dir/ai_runtime" ]; then
    mkdir ai_runtime

mkdir $root_dir/ai_runtime/data -f
mkdir $root_dir/ai_runtime/etc -f
mkdir $root_dir/ai_runtime/bin -f
mkdir $root_dir/ai_runtime/bin/service -f
mkdir $root_dir/ai_runtime/bin/ai -f
mkdir $root_dir/ai_runtime/log -f

# 基本配置文件
if [ ! -f "$root_dir/ai_runtime/etc/sys_capability.json" ]; then
    cp ai_platform/etc/sys_capability.json $root_dir/ai_runtime/etc/ -r

if [ ! -f "$root_dir/ai_runtime/etc/sys_config.json" ]; then
    cp ai_platform/etc/sys_config.json $root_dir/ai_runtime/etc/ -r

cp ai_platform/bin/* $root_dir/ai_runtime/bin -rf
cp ai_platform/ai_run.sh $root_dir/ -f

# 特殊功能脚本