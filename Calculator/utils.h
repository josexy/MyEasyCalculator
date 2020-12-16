#pragma once
#include <bitset>
#include <sstream>
#include <string>

using std::bitset;
using std::string;
using std::stringstream;

#define UnsignedInteger unsigned long

template <size_t N = 8>
struct convert_binary {
    // 十进制转二进制字符串
    string operator()(UnsignedInteger v) {
        bitset<N> bs(v);
        return bs.to_string();
    }
    // 二进制字符串转十进制
    UnsignedInteger operator()(const string& s) {
        bitset<N> bs(s);
        return bs.to_ulong();
    }
};

// 将函数名转化为小写
inline string toLower(string& s) {
    for (int i = 0; i < s.size(); i++) s[i] = tolower(s[i]);
    return s;
}

// 其他进制数转换为10进制整数
template <class T, size_t Base = 10>
inline T toBase(const string& s) {
    stringstream ss;
    if (Base == 16)
        ss << hex << s;
    else if (Base == 8)
        ss << oct << s;
    else if (Base == 2)
        return bitset<32>(s).to_ulong();
    T v;
    ss >> v;
    return v;
}

template <class T>
inline T toAny(const string& s) {
    stringstream ss;
    ss << s;
    T t;
    ss >> t;
    return t;
}