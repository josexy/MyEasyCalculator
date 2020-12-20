#pragma once

#include <functional>
#include <unordered_map>

#include "Reader.h"
using std::unordered_map;

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


// PS: 其他进制数在词法解析时转化为10进制
enum class Tag {
    Number,          // 十进制数
    Float,           // 浮点数
    Identifier,      // 标识符,变量名
    And,             // &
    Or,              // |
    Not,             // !
    Xor,             // ^
    Negate,          // ~
    Equal,           // =
    Add,             // +
    Sub,             // -
    Mul,             // *
    Div,             // /
    Mod,             // %
    ShiftLeft,       // <<
    ShiftRight,      // >>
    Pow,             // **
    Function,        // 一元函数 sin(x)
    BinaryFunction,  // 二元函数 pow(x,y)
    END_SEP,         // 变量分隔符 ;
    BEGIN_FUNC,      // 函数定义的开始 f(
    END_FUNC,        // 函数定义的结束 )
    BEGIN_BRACKET,   // 一般的左括号
    END_BRACKET,     // 一般的右括号
    Other            // 其他不需要解析的字符，比如二元函数的,
};

class Token {
   public:
    // tag符号表
    unordered_map<Tag, string> tagTable = {
        {Tag::And, "&"},         {Tag::Or, "|"},
        {Tag::Not, "!"},         {Tag::Xor, "^"},
        {Tag::Negate, "~"},      {Tag::Add, "+"},
        {Tag::Sub, "-"},         {Tag::Mul, "*"},
        {Tag::Div, "/"},         {Tag::Equal, "="},
        {Tag::Mod, "%"},         {Tag::ShiftLeft, "<<"},
        {Tag::ShiftRight, ">>"}, {Tag::Pow, "**"},
        {Tag::END_SEP, ";"},     {Tag::BEGIN_BRACKET, "("},
        {Tag::END_BRACKET, ")"}};

    Token(Tag tag) : __tag(tag) {}
    Token(char c) : __tag(Tag::Other), __c(c) {}
    virtual ~Token() {}
    virtual Tag type() { return __tag; }
    virtual string toString() {
        if (tagTable.find(__tag) != tagTable.end())
            return tagTable[__tag];
        else if (__tag == Tag::Other) {
            return string(1, static_cast<char>(__c));
        } else
            return "";
    }

   private:
    Tag __tag;
    char __c;
};

// 整数
class Number : public Token {
   public:
    Number(Integer value) : Token(Tag::Number), __value(value) {}
    Tag type() { return Tag::Number; }
    string toString() { return to_string(__value); }
    Integer value() { return __value; }

   private:
    Integer __value;
};

// 浮点数
class Float : public Token {
   public:
    Float(double value) : Token(Tag::Float), __value(value) {}
    Tag type() { return Tag::Float; }
    string toString() { return to_string(__value); }
    double value() { return __value; }

   private:
    double __value;
};

// 变量
class Word : public Token {
   public:
    Word(const string& word, bool minus = false, Tag tag = Tag::Identifier)
        : Token(tag), __lexeme(word), __minus(minus) {}
    Tag type() { return Tag::Identifier; }

    virtual string toString() { return lexeme(); }
    virtual string lexeme() { return __lexeme; }
    virtual bool negative() { return __minus; }

   protected:
    string __lexeme;
    bool __minus;
};
// 一元函数
class Function : public Word {
   public:
    Function(const string& name, bool minus = false)
        : Word(name, minus, Tag::Function) {}
    Tag type() override { return Tag::Function; };
};
// 二元函数
class BinaryFunction : public Word {
   public:
    BinaryFunction(const string& name, bool minus = false)
        : Word(name, minus, Tag::Function) {}
    Tag type() override { return Tag::BinaryFunction; };
};
