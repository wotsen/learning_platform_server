# `SDK`设计文档

# 目录

[TOC]

## 1. 目录说明

- `sdk_network`为`sdk`网络
- `sdk_protocol`为`sdk`协议
- `sdk_tree`为`sdk`节点树

## 2.`sdk`协议详细设计

### 2.1 总体说明

协议部分采用`google`的开源`protobuf`，当前使用版本`3`，有点在于跨平台、效率高、接口丰富。

参考`http`协议`REST`架构。

```protobuf
message Sdk {
    Header header = 1;      ///< 协议头
    Body body = 2;          ///< 协议内容
    Footer footer = 3;      ///< 协议脚标，只有返回有用
}
```

### 2.2 协议头

```protobuf
message Header {
    // 魔术数字
    SdkMagic msg_magic = 1;
    // 版本，必须校验SDK_CUR_VERSION
    SdkVersion version = 2;
    // 包序号
    int64 pack_id = 3;
    // 数据时间
    DataTime time = 4;
    // 数据方向
    DataFlow data_dir = 5;
    // 源地址
    Host host = 6;
    // 目的地址
    Host dest = 7;
    // 传输协议
    TransProto trans_proto = 8;
}
```

### 2.3. 主体部分

与`http`有区别的地方在于，路径及方法在主题部分：

```protobuf
// 请求动作，参考http实现
enum OperationType {
    INVALID = 0;            // 无效
    GET = 1;                // get用于获取资源
    POST = 2;               // 用于上传资源，主要是存储，比如升级，上传文件等
    PUT = 4;                // 用于资源状态设置
    DELETE = 8;             // 用于删除资源
}

message Body {
    UserSessionMsg user_session = 1;
    string url = 2;  // 请求路径，所有请求的响应使用路径与函数映射
    OperationType method = 3;
    Content content = 5;  // 数据内容，登录等处时并不需要
}
```

`user_session`表示用户信息。

`url`表示资源路径，`method`表示方法，必须严格遵守`OperationType`说明。

`content`才是业务内容的数据。

### 2.4 业务内容数据

```protobuf
// 数据内容
message Content {
    // 占位
    int64 magic = 1;

    // 所有的业务数据协议放在此处,每条消息最多只有一个字段被赋值
	oneof ContentOneof {
        /********************* 管理系统协议内容 ***************************/

        // 模块列表，支持get/put
        AppModuleCoutInfoList app_module_list = 2;
        // 用户列表，支持get/put/delete
        UserList user_list = 3;
        
        /*********************** 开放协议内容 *****************************/
	}
}
```

所有数据放在`oneof ContentOneof`当中，每次都只能由一个数据被设置，因此不同业务内容的协议必须单独定义。另外不允许定义在此处，必须单独编译协议内容，导入在此处：

```protobuf
import "in_sdk_body_user.proto";
import "in_sdk_body_appmodule.proto";
```

### 2.5 协议返回结果

```protobuf
// 详细业务返回码，所有业务返回码定义在此处
enum ContentResultE {
    // 成功
    R_CODE_OK = 0;
    // 失败
    R_CODE_ERROR = 1;
    // 用户已经存在
    R_CODE_USER_EXIST = 2;
    // 用户token超时
    R_CODE_USER_TOKEN_TIMEOUT = 3;
    // 用户不存在
    R_CODE_USER_NOT_EXIST = 4;
    // 用户黑名单
    R_CODE_USER_IN_BLACK_LIST = 5;
    // 用户权限不足
    R_CODE_USER_NO_PERMISSION = 6;
    // 用户密码错误
    R_CODE_USER_PASS_ERROR = 7;
    // 用户生存时间过长，不支持
    R_CODE_USER_ALIVE_TIME_TOOLONG = 8;
}

// 消息协议响应结果
enum ResponseResult {
    OK = 0;         // 正常响应
    ERROR = 1;      // 异常响应
}

// 协议响应结果
message SdkResult {
    ResponseResult status_code = 1; // 响应结果状态码，发送方并不需要
    string code = 2;                // 响应短语
}

// 具体业务响应结果
message ContentResult {
    ContentResultE status_code = 1; // 响应结果状态码
    string code = 2;                // 响应短语
}

// 结果
message Result {
    // sdk响应结果
    SdkResult sdk_result = 1;
    // 业务响应结果
    ContentResult content_result = 2;
}

message Footer {
    int32 res = 1;      // 保留
    Result result = 2;  // 返回结果，发送方不需要
}
```

响应结果只有响应时有用，分`sdk`响应结果，与具体业务无关；`content_result`由具体业务自身填写返回结果。具体的业务结果内容仍然在主体协议内。

业务返回码`ContentResultE`统一放在此处，优点在于便于管理，缺陷在于过多返回码难以阅读。考虑折中方式。

