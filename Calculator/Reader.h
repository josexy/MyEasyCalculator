#pragma once

#include <cstring>
#include <fstream>

#include "utils.h"
using std::ifstream;
using std::string;

class Reader {
   public:
    Reader(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader& operator=(Reader&&) = delete;

    Reader() {}
    explicit Reader(const string& str) { set_buffer(str); }

    explicit Reader(ifstream& ifs) {
        char c;
        while (~(c = ifs.get())) __sbuffer[__len++] = c;
    }
    ~Reader() {
        if (__sbuffer) delete[] __sbuffer;
    }
    void set_buffer(const string& s) {
        if (__sbuffer) {
            delete[] __sbuffer;
            __sbuffer = nullptr;
        }
        __len = s.length();
        __ptr = -1;
        __sbuffer = new char[__len]{0};
        memcpy(__sbuffer, s.data(), __len * sizeof(char));
    }
    // 当前的字符
    char cur() {
        if (__ptr <= -1 || __ptr >= __len) return EOF;
        return __sbuffer[__ptr];
    }
    // 移动并获取下一个字符
    char get() {
        if (++__ptr >= __len) return EOF;
        return __sbuffer[__ptr];
    }
    // 不移动并返回上一个字符
    char backc() {
        if (__ptr - 1 < 0) return EOF;
        return __sbuffer[__ptr - 1];
    }
    // 回退一个字符
    void back() { --__ptr; }

    // 获取下一个字符
    char peek() {
        if (__ptr + 1 >= __len) return EOF;
        return __sbuffer[__ptr + 1];
    }
    // 如果下一个字符与指定的字符相同则移动，否则不移动
    pair<bool, char> geteq(int c) {
        char x = get();
        // 如果一致，则不需要回退
        if (x == c) return make_pair(true, c);
        // 注意，一定要回退
        back();
        return make_pair(false, c);
    }
    // 从当前位置开始获取一定数目的字符串
    string gets(int count) {
        string s;
        for (int i = 0; i < count; i++) s.append(1, get());
        return s;
    }
    int len() { return __len; }
    int pos() { return __ptr; }
    bool eof() { return __ptr > __len - 1; }

    void reset() { __ptr = -1; }
    void clear() {
        reset();
        memset(__sbuffer, 0, __len * sizeof(char));
    }

   private:
    char* __sbuffer = nullptr;
    int __ptr = -1, __len = 0;
};