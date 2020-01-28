# `hooks`脚本

## `git hooks`脚本

> - `pre-commit.py`，用于限制`git commit`，会检查程序构建，如果构建不过会终止提交
> - `commit-msg.py`，用于自动添加版本信息和程序校验码.

**由于构建和计算校验码会比较慢，因此提交时会有延迟.**

## 安装

直接运行:
```shell script
./hooks-setup.py
```