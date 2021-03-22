#ifndef MYEASYCALCULATOR_READER_H
#define MYEASYCALCULATOR_READER_H

#include <cstring>
#include <fstream>

#include "utils.h"

namespace calculator {

class Reader {
   public:
    Reader(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader& operator=(Reader&&) = delete;

    Reader() {}
    explicit Reader(const std::string& str) { set_buffer(str); }

    explicit Reader(std::ifstream& ifs) {
        char c;
        while (~(c = ifs.get())) sbuffer_[len_++] = c;
    }
    ~Reader() {
        if (sbuffer_) delete[] sbuffer_;
    }
    void set_buffer(const std::string& s) {
        if (sbuffer_) {
            delete[] sbuffer_;
            sbuffer_ = nullptr;
        }
        len_ = (int)s.length();
        ptr_ = -1;
        sbuffer_ = new char[len_]{0};
        memcpy(sbuffer_, s.data(), len_ * sizeof(char));
    }
    // 当前的字符
    char cur() const {
        if (ptr_ <= -1 || ptr_ >= len_) return EOF;
        return sbuffer_[ptr_];
    }
    // 移动并获取下一个字符
    char get() {
        if (++ptr_ >= len_) return EOF;
        return sbuffer_[ptr_];
    }
    // 不移动并返回上一个字符
    char backc() const {
        if (ptr_ - 1 < 0) return EOF;
        return sbuffer_[ptr_ - 1];
    }
    // 回退一个字符
    void back() { --ptr_; }

    // 获取下一个字符
    char peek() const {
        if (ptr_ + 1 >= len_) return EOF;
        return sbuffer_[ptr_ + 1];
    }
    // 如果下一个字符与指定的字符相同则移动，否则不移动
    std::pair<bool, char> geteq(int c) {
        char x = get();
        // 如果一致，则不需要回退
        if (x == c) return std::make_pair(true, c);
        // 注意，一定要回退
        back();
        return std::make_pair(false, c);
    }
    // 从当前位置开始获取一定数目的字符串
    std::string gets(int count) {
        std::string s;
        for (int i = 0; i < count; i++) s.append(1, get());
        return s;
    }
    int len() const { return len_; }
    int pos() const { return ptr_; }
    bool eof() const { return ptr_ > len_ - 1; }

    void reset() { ptr_ = -1; }
    void clear() {
        reset();
        memset(sbuffer_, 0, len_ * sizeof(char));
    }

   private:
    char* sbuffer_ = nullptr;
    int ptr_ = -1, len_ = 0;
};

}  // namespace calculator
#endif