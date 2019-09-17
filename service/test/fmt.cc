/*
 * @Date: 2019-08-24 20:15:04
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-24 20:15:59
 */
#include "../tools/fmt/format.h"
#include "../tools/fmt/printf.h

int main(void)
{
    fmt::print("Hello, {}!\n", "world");  // Python-like format string syntax
    fmt::printf("Hello, %s!\n", "world");  // Python-like format string syntax
    return 0;
}