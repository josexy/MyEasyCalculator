#ifndef MYEASYCALCULATOR_UTILS_H
#define MYEASYCALCULATOR_UTILS_H
#include <bitset>
#include <sstream>
#include <string>

namespace calculator {

#define UnsignedInteger unsigned long

#ifdef WIN32
using Integer = int64_t;
#define __xpow powl
#define __xsqrt sqrtl
#define __xceil ceill
#define __xcos cosl
#define __xsin sinl
#define __xtan tanl
#define __xlog logl
#define __xlog2 log2l
#define __xlog10 log10l
#define __xfloor floorl
#define __xasin asinl
#define __xacos acosl
#define __xatan atanl
#define __xexp expl
#define __xround roundl
#define __xerf erfl
#else
using Integer = __int64_t;
#define __xpow powf64
#define __xsqrt sqrtf64
#define __xceil ceilf64
#define __xcos cosf64
#define __xsin sinf64
#define __xtan tanf64
#define __xlog logf64
#define __xlog2 log2f64
#define __xlog10 log10f64
#define __xfloor floorf64
#define __xasin asinf64
#define __xacos acosf64
#define __xatan atanf64
#define __xexp expf64
#define __xround roundf64
#define __xerf erff64
#endif

template <size_t N = 8>
struct convert_binary {
    // 十进制转二进制字符串
    std::string operator()(UnsignedInteger v) {
        std::bitset<N> bs(v);
        return bs.to_string();
    }
    // 二进制字符串转十进制
    UnsignedInteger operator()(const std::string& s) {
        std::bitset<N> bs(s);
        return bs.to_ulong();
    }
};

// 将函数名转化为小写
inline std::string toLower(std::string& s) {
    for (int i = 0; i < s.size(); i++) s[i] = tolower(s[i]);
    return s;
}

// 其他进制数转换为10进制整数
template <class T, size_t Base = 10>
inline T toBase(const std::string& s) {
    std::stringstream ss;
    if (Base == 16)
        ss << std::hex << s;
    else if (Base == 8)
        ss << std::oct << s;
    else if (Base == 2)
        return std::bitset<32>(s).to_ulong();
    T v;
    ss >> v;
    return v;
}

template <class T>
inline T toAny(const std::string& s) {
    std::stringstream ss;
    ss << s;
    T t;
    ss >> t;
    return t;
}

// 一些辅助函数
inline bool ishex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}
inline bool isdigit(char c) { return c >= '0' && c <= '9'; }
inline bool isbin(char c) { return c == '0' || c == '1'; }
inline bool isoct(char c) { return c >= '0' && c <= '7'; }

inline bool isexponent(char c) { return c == 'e' || c == 'E'; }
inline bool isletter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
inline bool isnewline_(char c) { return c == '\n'; }
inline bool isspace(char c) {
    return c == '\0' || c == '\t' || c == '\v' || c == ' ';
}

}  // namespace calculator
#endif