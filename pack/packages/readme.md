# 发布包说明

## 打包格式

```shell
[update|patch]_[learn_platform]_package_[2020-01-27_00-00-00][r|d]_[v0.0.1]_[crc16].zip
```

- `[update|patch]`，`update`表示全量包，`patch`表示增量包.
- `[learn_platform]`，填写升级的具体模块名.当前可选：
  - `learn_platform`
  - `learn_platform_server`
  - `learn_platform_website`
  - `learn_platform_client`
- `[2020-01-27_00-00-00]`，日期格式精确到秒.
- `[r|d]`，发布包选择`r`，调试包选择`d`.
- `[v0.0.1]`，版本，版本增量参照`gnu`.
- `[crc16]`校验码

升级包必须进行加密.

