# 谷歌`protocol buffer`

## 谷歌开源库`protocol buffer`安装说明

[官网](https://developers.google.cn/protocol-buffers/)

获取源码：

注意安装`3.9.1`版本

```shell
$ git clone https://github.com/protocolbuffers/protobuf.git
$ cd protobuf
$ git submodule update --init --recursive
$ ./autogen.sh
```

编译：

```shell
$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig # refresh shared library cache.
```

### 其他语言安装

- `js`:

  ```shell
  $ cd js
  $ npm install
  $ npm test
  
  # If your protoc is somewhere else than ../src/protoc, instead do this.
  # But make sure your protoc is the same version as this (or compatible)!
  $ PROTOC=/usr/local/bin/protoc npm test
  ```

- `python`:

  ```shell
  $ cd python
  $ python setup.py install
  ```

更多可以查看官方文档。

## 使用

[参考](https://blog.csdn.net/sylar_d/article/details/51325987)

协议的目的在于使用一个配置文件，生成多种语言适配的协议源码。

优点是跨语言，快速；缺点是只能通过`.proto`来了解协议。

### 基本示例

```protobuf
syntax =“proto3”;  // 这个是版本号，不指定的话默认是proto2，生成代码时也会这样解析

/*
你可以使用这种注释
*/

message LoginReqMessage {
    required int64 acctID = 1;
    required string passwd = 2;
}
```

- `message`是消息定义的关键字，等同于`c++`里面的`struct/class`;
- `LoginReqMessage`是消息(结构体名)；
- `required`前缀表示该字段为必要字段(默认)，表明在序列化和反序列化之前该字段都必须已经被赋值，另外的是：`optional`和`repeated`，他们没有这种限制，主要用于数组字段；
- `int64`和`string`表示长整形和字符串类型消息字段，可以指定其他的复合类型；
- `acctID`和`passwd`表示成员变量名；
- 标签数字**1**和**2**则表示不同的字段在序列化后的二进制数据中的布局位置。在该例中，`passwd`字段编码后的数据一定位于`acctID`之后。需要注意的是该值在同一`message`中不能重复。另外，对于`Protocol Buffer`而言，标签值为1到15的字段在编码时可以得到优化，既标签值和类型信息仅占有一个`byte`，标签范围是16到2047的将占有两个`bytes`，而Protocol Buffer可以支持的字段数量则为2的29次方减一。有鉴于此，我们在设计消息结构时，可以尽可能考虑让`repeated`类型的字段标签位于1到15之间，这样便可以有效的节省编码后的字节数量。但是有些字段是保留字段：19000-19999(`FieldDescriptor::kFirstReservedNumber`到`FieldDescriptor::kLastReservedNumber`)，使用的话会报错。

### 限定符

1. `required`，在每个消息中必须至少留有一个required类型的字段。，不写是默认的。

2. 每个消息中可以包含0个或多个optional类型的字段。

3. repeated表示的字段可以包含0个或多个数据。需要说明的是，这一点有别于C++/Java中的数组，因为后两者中的数组必须包含至少一个元素。

4. 如果打算在原有消息协议中添加新的字段，同时还要保证老版本的程序能够正常读取或写入，那么对于新添加的字段必须是optional或repeated。道理非常简单，老版本程序无法读取或写入新增的required限定符的字段。

5. `reserved`保留。主要是用于版本兼容性，表明可能以后会使用到：

   ```protobuf
   enum Foo {
     reserved 2, 15, 9 to 11, 40 to max;  // 枚举值，max表示到最大
     reserved "FOO", "BAR";  // 这几个名称不要使用
   }
   ```

### 类型表

| .proto中的类型 | 注意                                                         | C++中类型 | java中类型 |
| -------------- | ------------------------------------------------------------ | --------- | ---------- |
| double         |                                                              | double    | double     |
| float          |                                                              | float     | float      |
| int32          | 自动调整编码长度，如果需要保存负数，请使用sint32             | int32     | int        |
| int64          | 自动调整编码长度，如果需要保存负数，请使用sint64             | int64     | long       |
| uint32         | 自动调整编码长度                                             | uint32    | int        |
| uint64         | 自动调整编码长度                                             | uint64    | long       |
| sint32         | 自动调整编码长度，表示有符号数，负数的编码效率高于int32      | int32     | int        |
| sint64         | 自动调整编码长度，表示有符号数，负数的编码效率高于int64      | int64     | long       |
| fixed32        | 固定使用4bytes编码，在编码大数(228228)的时候比uint32更有效率 | int32     | int        |
| fixed32        | 固定使用8bytes编码，在编码大数(256256)的时候比uint64更有效率 | int42     | long       |
| sfixed32       | 固定使用4bytes编码                                           | int32     | int        |
| sfixed64       | 固定使用8bytes编码                                           | int64     | long       |
| bool           |                                                              | bool      | boolean    |
| string         | string只能包含UTF-8和7-bit ASCII文本                         | string    | String     |
| bytes          | 包含任意长度的bytes                                          | string    | ByteString |

### 枚举

```protobuf
//在定义Protocol Buffer的消息时，可以使用和C++/Java代码同样的方式添加注释。
enum UserStatus {
	OFFLINE = 0;  //表示处于离线状态的用户
	ONLINE = 1;   //表示处于在线状态的用户
}
message UserInfo {
	required int64 acctID = 1;
	required string name = 2;
	required UserStatus status = 3;
}
```

- `enum`就是枚举类型；
- 注意每个枚举值之间是分号分割的；
- `OFFLINE/ONLINE`为枚举值；
- 0和1表示枚举值所对应的实际整型值，和C/C++一样，可以为枚举值指定任意整型值，而无需总是从0开始定义。

### 默认值

- `string`为空
- `bytes`为空
- `bool`为`false`
- 数值为0
- 枚举，第一个

### 嵌套

```protobuf
enum UserStatus {
	OFFLINE = 0;
	ONLINE = 1;
}
message UserInfo {
	required int64 acctID = 1;
	required string name = 2;
	required UserStatus status = 3;
}
message LogonRespMessage {
	required LoginResult logonResult = 1;
	required UserInfo userInfo = 2;
}

message SearchRequest {
  string query = 1;
  int32 page_number = 2;
  int32 result_per_page = 3;
  enum Corpus {
    UNIVERSAL = 0;
    WEB = 1;
    IMAGES = 2;
    LOCAL = 3;
    NEWS = 4;
    PRODUCTS = 5;
    VIDEO = 6;
  }
  Corpus corpus = 4;
  optional Corpus corpus = 5 [default = UNIVERSAL];
}

enum EnumAllowingAlias {
  option allow_alias = true;  // 允许别名
  UNKNOWN = 0;
  STARTED = 1;  // 下面这两个都是用一个
  RUNNING = 1;
}
enum EnumNotAllowingAlias {
  UNKNOWN = 0;
  STARTED = 1;
  // RUNNING = 1;  // Uncommenting this line will cause a compile error inside Google and a warning message outside.
}

message SearchResponse {
  message Result {
    string url = 1;
    string title = 2;
    repeated string snippets = 3;
  }
  repeated Result results = 1;
}

message SomeOtherMessage {
  SearchResponse.Result result = 1;
}
```

另外也可以使用导入其他文件的方式：

`import "myproject/CommonMessages.proto";`和`import public "myproject/CommonMessages.proto"`，其中`public`表示如果被其他文件引用，那么会传递下去。

### 扩展

`Extensions`可以让我们在`Messag`e中定义一些域，这些域可以交由第三方进行扩展我们定义的`Message`:

```protobuf
message Foo{
    //.....
    extensions 100 to 199;
}

extend Foo{
    optional int32 bar = 123;
}

// 嵌套的扩展
message Baz{
    extend Foo{
        optional int32 bar = 126;
	}
    ...
}

// 但是这种方式更好理解一些
message Baz{
    extend Foo{
        optional Baz foo_ext = 127;
    }
}
```

### 类似`union`

如果我们定义的`Message`包含很多`optional`域，但是我们仅仅会`set`这些`optional`中的最多一个。我们就可以使用`oneof`来保证这种`set`机制，并且会节省`memory`。（注：并不是说`optional`域在`oneof`中，这里只是举个例子）

`oneof`域和`optional`域仅有一个不同，就是`oneof`域中的所有域是共享`memory`的，并且每一次只能够设置一个域，我们在设置`oneof`中的任意一个域，将会自动清除其他设置。（很容易理解，因为只有一个使用空间）。如果`oneof`域中设置了其中一个域的时候，使用case()或者是``WhichOneof()`方法（这要根据我们使用的语言选择）来查看这个域。

```protobuf
message SampleMessage{
    oneof test_oneof{
        string name = 4;
        SubMessage sub_message = 9;
    }
}
```

我们可以在Oneof域中定义oneof域，并且可以在oneof中添加任意的域，但是不可以使用required，optional，repeated关键字（但是其中出现的自定义结构可以含有这些）。

- 如果解析器在一个oneof中解析到了多个值，那么仅仅会使用最后一个解析值
- extensions不支持oneof
- oneof域中不可以出现repeated
- 反射机制的API可以在oneof域上工作
- 如果使用C++编程，需要注意避免代码出现memory crashes。以下代码会崩溃，因为sub_message在调用set_name()的时候已经删除。

### 升级说明

升级可能并不能保证所有的端都能及时得到升级，所以要考虑兼容性。

- 不要修改已有字段的标签号
- 新加字段必须是`optional`和`repeated`，否则无法相互通信
- 不能移除`required`字段，`optional`和`repeated`可以移除，但是标签号不能移除，总之字节序不能乱。
- ` int32`、`uint32`、`int64`、`uint64`和`bool`等类型之间是兼容的，`sint32`和`int64`是兼容的，`string`和`bytes`是兼容的，`fixed32`和`sfixed32`，以及`fixed64`和`sfixed64`之间是兼容的，这意味着如果想修改原有字段的类型时，为了保证兼容性，只能将其修改为与其原有类型兼容的类型，否则就将打破新老消息格式的兼容性。数据类型兼容其实是他们的内部长度是一样的。
- optional和repeated限定符也是相互兼容的。

### 包

```protobuf
package ourproject.lyphone;  // 这个其实是名称空间，c++会得到namespace ourproject{会得到namespace lyphone {}}，java会得到包名
```

### 选项

`Protocol Buffer`允许我们在`.proto`文件中定义一些常用的选项，这样可以指示`Protocol Buffer`编译器帮助我们生成更为匹配的目标语言代码。`Protocol Buffer`内置的选项被分为以下三个级别：

- 文件级别，这样的选项将影响当前文件中定义的所有消息和枚举
- 消息级别，这样的选项仅影响某个消息及其包含的所有字段
- 字段级别，这样的选项仅仅响应与其相关的字段

#### 常用选项

- `option java_package = "com.companyname.projectname";`**java_package**是文件级别的选项，通过指定该选项可以让生成Java代码的包名为该选项值，如上例中的Java代码包名为`com.companyname.projectname`。与此同时，生成的Java文件也将会自动存放到指定输出目录下的`com/companyname/projectname`子目录中。如果没有指定该选项，Java的包名则为package关键字指定的名称。该选项对于生成C++代码毫无影响。
- `option java_outer_classname = "LYPhoneMessage";`**java_outer_classname**是文件级别的选项，主要功能是显示的指定生成Java代码的外部类名称。如果没有指定该选项，Java代码的外部类名称为当前文件的文件名部分，同时还要将文件名转换为驼峰格式，如：`my_project.proto`，那么该文件的默认外部类名称将为`MyProject`。该选项对于生成C++代码毫无影响。
        注：主要是因为Java中要求同一个.java文件中只能包含一个Java外部类或外部接口，而C++则不存在此限制。因此在`.proto`文件中定义的消息均为指定外部类的内部类，这样才能将这些消息生成到同一个Java文件中。在实际的使用中，为了避免总是输入该外部类限定符，可以将该外部类静态引入到当前Java文件中，如：*import static com.company.project.LYPhoneMessage.*。
- `option optimize_for = LITE_RUNTIME;`**optimize_for**是文件级别的选项，Protocol Buffer定义三种优化级别SPEED/CODE_SIZE/LITE_RUNTIME。缺省情况下是SPEED。
  - SPEED: 表示生成的代码运行效率高，但是由此生成的代码编译后会占用更多的空间。
  - CODE_SIZE: 和SPEED恰恰相反，代码运行效率较低，但是由此生成的代码编译后会占用更少的空间，通常用于资源有限的平台，如Mobile。
  - LITE_RUNTIME: 生成的代码执行效率高，同时生成代码编译后的所占用的空间也是非常少。这是以牺牲Protocol Buffer提供的反射功能为代价的。因此我们在C++中链接Protocol Buffer库时仅需链接libprotobuf-lite，而非libprotobuf。在Java中仅需包含protobuf-java-2.4.1-lite.jar，而非protobuf-java-2.4.1.jar。
          注：对于LITE_MESSAGE选项而言，其生成的代码均将继承自MessageLite，而非Message。
- [**pack** = true]: 因为历史原因，对于数值型的repeated字段，如int32、int64等，在编码时并没有得到很好的优化，然而在新近版本的Protocol Buffer中，可通过添加[pack=true]的字段选项，以通知Protocol Buffer在为该类型的消息对象编码时更加高效。如：
        repeated int32 samples = 4 [packed=true]。
        注：该选项仅适用于**2.3.0**以上的Protocol Buffer。
- [**default** = default_value]: optional类型的字段，如果在序列化时没有被设置，或者是老版本的消息中根本不存在该字段，那么在反序列化该类型的消息是，optional的字段将被赋予类型相关的缺省值，如bool被设置为false，int32被设置为0。Protocol Buffer也支持自定义的缺省值，如：
        optional int32 result_per_page = 3 [default = 10]。

### 编译

```shell
protoc --proto_path=IMPORT_PATH --cpp_out=DST_DIR --java_out=DST_DIR --python_out=DST_DIR path/to/file.proto
```

- `protoc`为`Protocol Buffer`提供的命令行编译工具。
- `--proto_path`等同于-I选项，主要用于指定待编译的`.proto`消息定义文件所在的目录，该选项可以被同时指定多个。
- `--cpp_out`选项表示生成C++代码，`--java_out`表示生成Java代码，`--python_out`则表示生成Python代码，其后的目录为生成后的代码所存放的目录。
- `path/to/file.proto`表示待编译的消息定义文件。

注：对于C++而言，通过Protocol Buffer编译工具，可以将每个`.proto`文件生成出一对.h和.cc的C++代码文件。生成后的文件可以直接加载到应用程序所在的工程项目中。如：`MyMessage.proto`生成的文件为`MyMessage.pb.h`和`MyMessage.pb.cc`。