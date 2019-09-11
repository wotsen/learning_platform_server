/*
 * @Date: 2019-08-11 14:27:30
 * @LastEditors: 余王亮
 * @LastEditTime: 2019-08-11 14:37:22
 */
/**
 * @file tfile.cpp
 * @author 余王亮 (wotsen@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2019-08-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "tfile.h"

namespace tfile {

template <> const char* newlineString<Newline::atari8>() { return "\x9b"; }
template <> const char* newlineString<Newline::cr>() { return "\r"; }
template <> const char* newlineString<Newline::cr_lf>() { return "\r\n"; }
template <> const char* newlineString<Newline::lf>() { return "\n"; }
template <> const char* newlineString<Newline::lf_cr>() { return "\n\r"; }
template <> const char* newlineString<Newline::nl>() { return "\x15"; }
template <> const char* newlineString<Newline::rs>() { return "\x1e"; }
template <> const char* newlineString<Newline::zx8x>() { return "\x76"; }

template <> const char* modeString<Mode::read>() { return "r"; }
template <> const char* modeString<Mode::readWrite>() { return "r+"; }
template <> const char* modeString<Mode::write>() { return "w"; }
template <> const char* modeString<Mode::truncate>() { return "w+"; }
template <> const char* modeString<Mode::append>() { return "a"; }
template <> const char* modeString<Mode::readAppend>() { return "a+"; }

template <typename SizeFunction>
void testableRead(const char* filename, std::string& s, SizeFunction size) {
    // The file size might change between getting the size and reading it: see
    // https://www.reddit.com/r/cpp/comments/aiprv9/tiny_file_utilities/eerbyza/

    s.resize(size(filename));

    Reader reader(filename);
    s.resize(reader.read(s));

    while (true) {
        static const auto BUFFER_SIZE = 16;
        char buffer[BUFFER_SIZE];
        if (auto bytes = reader.read(buffer, BUFFER_SIZE))
            s.insert(s.end(), buffer, buffer + bytes);
        else
            break;
    }
}

template <typename Container>
size_t writeLines(const char* filename, Container c) {
    return Writer(filename).writeLines().write(c);
}

template <typename ForwardIt>
size_t writeLines(const char* filename, ForwardIt begin, ForwardIt end) {
    return Writer(filename).writeLines().write(begin, end);
}

}