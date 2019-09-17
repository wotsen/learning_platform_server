/*
 * @Date: 2019-08-24 19:30:14
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-24 19:31:17
 */
#include <iostream>
#include <boost/format.hpp>

using std::cout;
using boost::format;

int main(void)
{
// 基于“位置指示符”的格式串语法
cout << format("%1% %2% %3% %2% %1%\n") % "11" % "22" % "333";
// 输出如下：
// 11 22 333 22 11

// 兼容 printf() 的格式串语法
cout << format("%s  %d\n") % "price" % 1234;
// 输出如下：
// price  1234
    return 0;
}