#ifndef MYEASYCALCULATOR_LEXER_H
#define MYEASYCALCULATOR_LEXER_H
#include <cmath>
#include <memory>
#include <stack>

#include "Exception.h"
#include "Token.h"

namespace calculator {

using UnaryFunctionType = std::function<double(double)>;
using BinaryFunctionType = std::function<double(double, double)>;

// 词法分析器,将输入的表达式转化成token序列
class Lexer {
   public:
    // 常量表
    std::unordered_map<std::string, double> constant;
    // 变量表
    std::unordered_map<std::string, std::shared_ptr<Token>> variable;
    // 一元函数
    std::unordered_map<std::string, UnaryFunctionType> unary_functions = {
        {"sqrt", __xsqrt},
        {"ceil", __xceil},
        {"cos", __xcos},
        {"sin", __xsin},
        {"tan", __xtan},
        {"log", __xlog},
        {"floor", __xfloor},
        {"acos", __xacos},
        {"asin", __xasin},
        {"atan", __xatan},
        {"exp", __xexp},
        {"log2", __xlog2},
        {"log10", __xlog10},
        {"erf", __xerf},
        {"round", __xround},
        {"factorial", [](double x) {
             int v = 1;
             for (int i = 1; i <= x; i++) v *= i;
             return v;
         }}};
    // 二元函数
    std::unordered_map<std::string, BinaryFunctionType> binary_functions = {
        {"pow", __xpow},
        {"max", [](double x, double y) { return x > y ? x : y; }},
        {"min", [](double x, double y) { return x < y ? x : y; }}};

   public:
    Lexer();
    explicit Lexer(const std::string& text) : reader_(text) { Lexer(); }

    void scan();

    // 是否是一元函数
    bool isUnaryFunction(const std::string& func) {
        return (unary_functions.find(func) != unary_functions.end());
    }
    // 是否是二元函数
    bool isBinaryFunction(const std::string& func) {
        return (binary_functions.find(func) != binary_functions.end());
    }
    // 保存变量名
    void putWord(const std::string& id) {
        if (variable.find(id) == variable.end())
            variable[id] = std::shared_ptr<Token>(new Word(id));
    }
    // 原始的常量名+用户定义的变量名
    void putConstant(const std::string& key, double value) {
        constant[key] = value;
        putWord(key);
    }

    Reader& reader() { return reader_; }
    std::vector<std::shared_ptr<Token>>& tokenList() { return tokenlist_; }
    std::stack<bool>& bm() { return bracket_match_; }

   private:
    int line_;
    char lookforward_;
    bool is_function_;

    Reader reader_;
    // token列表
    std::vector<std::shared_ptr<Token>> tokenlist_;
    // 用于识别函数的声明定义左右括号的位置
    // 同时还可以检测表达式中括号是否匹配(栈不为空)
    std::stack<bool> bracket_match_;
};
}  // namespace calculator
#endif