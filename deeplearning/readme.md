# 说明

暂时计划图像识别：模型训练，目标识别，图像文字识别。`python`/`cpp`实现。

`tensorflow`训练目标识别模型，`OpenCV`加载运行模型；`tesseract-ocr`做文字识别。

## python环境

这里使用ubuntu18

### 库安装

#### 图形库

- `pip install pytesseract`
- `pip install pillow`

#### `tesseract-ocr`引擎

​	光学字符识别(OCR,Optical Character Recognition)是指对文本资料进行扫描，然后对图像文件进行分析处理，获取文字及版面信息的过程。OCR技术非常专业，一般多是印刷、打印行业的从业人员使用，可以快速的将纸质资料转换为电子资料。关于中文OCR，目前国内水平较高的有清华文通、汉王、尚书，其产品各有千秋，价格不菲。国外OCR发展较早，像一些大公司，如IBM、微软、HP等，即使没有推出单独的OCR产品，但是他们的研发团队早已掌握核心技术，将OCR功能植入了自身的软件系统。对于我们程序员来说，一般用不到那么高级的，主要在开发中能够集成基本的OCR功能就可以了。这两天我查找了很多免费OCR软件、类库，特地整理一下，今天首先来谈谈Tesseract，下一次将讨论下Onenote 2010中的OCR API实现。可以在这里查看OCR技术的发展简史。
Tesseract的OCR引擎最先由HP实验室于1985年开始研发，至1995年时已经成为OCR业内最准确的三款识别引擎之一。然而，HP不久便决定放弃OCR业务，Tesseract也从此尘封。
​	数年以后，HP意识到，与其将Tesseract束之高阁，不如贡献给开源软件业，让其重焕新生－－2005年，Tesseract由美国内华达州信息技术研究所获得，并求诸于Google对Tesseract进行改进、消除Bug、优化工作。

[参考](https://github.com/tesseract-ocr/tesseract/wiki/Compiling#linux)

1. 安装`tesseract`

```shell
sudo apt install tesseract-ocr
```

如果希望安装开发工具执行命令

```shell
sudo apt install libtesseract-dev
```

Ubuntu系统需要安装如下依赖库

```shell
sudo apt-get install g++ # or clang++ (presumably) 
sudo apt-get install autoconf automake libtool 
sudo apt-get install pkg-config 
sudo apt-get install libpng-dev 
sudo apt-get install libjpeg8-dev 
sudo apt-get install libtiff5-dev 
sudo apt-get install zlib1g-dev
```

如果希望安装训练工具,需要安装下面的依赖库

```shell
sudo apt-get install libicu-dev 
sudo apt-get install libpango1.0-dev 
sudo apt-get install libcairo2-dev
```

2. 安装语言包

   [官网](https://github.com/tesseract-ocr/tessdata)

这里我只下载了`chi_tra.traineddata`和`chi_sim.traineddata`，下载后放到：`/usr/share/tesseract-ocr/4.00/tessdata/`，路径可能不一致，运行测试程序会有提示：

```python
import pytesseract
from PIL import Image

image = Image.open("test.png")
code = pytesseract.image_to_string(image,lang="chi_sim",config="-psm 6")
print(code)
```

可以通过`tesseract --help-psm` 查看`psm`:

```shell
➜  star tesseract --help-psm
Page segmentation modes:
  0    Orientation and script detection (OSD) only.
  1    Automatic page segmentation with OSD.
  2    Automatic page segmentation, but no OSD, or OCR.
  3    Fully automatic page segmentation, but no OSD. (Default)
  4    Assume a single column of text of variable sizes.
  5    Assume a single uniform block of vertically aligned text.
  6    Assume a single uniform block of text.
  7    Treat the image as a single text line.
  8    Treat the image as a single word.
  9    Treat the image as a single word in a circle.
 10    Treat the image as a single character.
 11    Sparse text. Find as much text as possible in no particular order.
 12    Sparse text with OSD.
 13    Raw line. Treat the image as a single text line,
       bypassing hacks that are Tesseract-specific.
```

0：定向脚本监测（OSD）
1： 使用OSD自动分页
2 ：自动分页，但是不使用OSD或OCR（Optical Character Recognition，光学字符识别）
3 ：全自动分页，但是没有使用OSD（默认）
4 ：假设可变大小的一个文本列。
5 ：假设垂直对齐文本的单个统一块。
6 ：假设一个统一的文本块。
7 ：将图像视为单个文本行。
8 ：将图像视为单个词。
9 ：将图像视为圆中的单个词。
10 ：将图像视为单个字符。
