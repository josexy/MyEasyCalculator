#pragma once
#include <cmath>
#include <memory>
#include <stack>

#include "Exception.h"
#include "Tokens.h"

// 词法分析器,将输入的表达式转化成token序列
class Lexer {
   public:
    // 常量表
    unordered_map<string, double> constantTable;
    // 变量表
    unordered_map<string, shared_ptr<Token>> variable;

    // 一元函数
    unordered_map<string, function<double(double)>> unary_functions = {
        {"sqrt", sqrtf},   {"ceil", ceilf}, {"cos", cosf},     {"sin", sinf},
        {"tan", tanf},     {"log", logf},   {"floor", floorf}, {"acos", acosf},
        {"asin", asinf},   {"atan", atanf}, {"exp", expf},     {"log2", log2f},
        {"log10", log10f}, {"erf", erff},   {"round", roundf}};

    // 二元函数
    unordered_map<string, function<double(double, double)>> binary_functions = {
        {"pow", powf},
        {"max", [](double x, double y) { return x > y ? x : y; }},
        {"min", [](double x, double y) { return x < y ? x : y; }}};

   public:
    Lexer() : line(0), lookforward(0), is_function(false) {
        constantTable.clear();
        variable.clear();
        tokenlist.clear();

        putConstant("pi", 3.141592653589793);
        putConstant("e", 2.718281828459045);
        putConstant("sqrt2", 1.4142135623730951);
    }

    Lexer(const string& text) : reader(text) { Lexer(); }

    void scan() {
        char c;
        // 标志是否为负数
        bool minus = false;
        // 跳过空白字符
        while (!reader.eof()) {
            c = reader.get();
            if (c == EOF) return;
            if (isspace(c))
                continue;
            else if (isnewline(c))
                line++;
            else if (c == '+' || c == '-') {
                char nc;
                // 判断是否出现多个连续的 +-符号
                while (isspace((nc = reader.get())))
                    ;
                if (c == nc) {
                    throw SyntaxError("can not present continue symbol ++/--");
                } else {
                    reader.back();
                    break;
                }
            } else
                break;
        }

        // )+   )- 向前看字符如果是 ) 说明当前的 +/- 一定是一个 减法或加法
        if (lookforward != ')' && !isexponent(lookforward) &&
            (c == '-' || c == '+')) {
            // +100 / -100  0.+100
            // 对于表达式 1 + -100 和 1. - 100 可以识别出来
            if (!isdigit(lookforward) && !isletter(lookforward) &&
                lookforward != '.') {
                if (c == '-') minus = true;
                c = reader.get();
                while (isspace(c)) c = reader.get();
            }
        }
        // 保存前一个字符
        lookforward = c;

        switch (c) {
            case '&':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::And)));
            case '|':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Or)));
            case '!':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Not)));
            case '^':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Xor)));
            case '~':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Negate)));
            case '+':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Add)));
            case '-': {
                // -表示一个负数，而非减法
                if (minus) break;
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Sub)));
            }
            case '*': {
                if (auto [ok, c] = reader.geteq('*'); ok) {
                    return tokenlist.push_back(
                        shared_ptr<Token>(new Token(Tag::Pow)));
                } else {
                    return tokenlist.push_back(
                        shared_ptr<Token>(new Token(Tag::Mul)));
                }
            }
            case '/':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Div)));
            case '=':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Equal)));
            case '%':
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::Mod)));
            case '<': {
                // <<左移
                if (auto [ok, c] = reader.geteq('<'); ok)
                    return tokenlist.push_back(
                        shared_ptr<Token>(new Token(Tag::ShiftLeft)));
                else
                    throw SyntaxError("unexpected <?");
            } break;
            case '>': {
                // >>右移
                if (auto [ok, c] = reader.geteq('>'); ok)
                    return tokenlist.push_back(
                        shared_ptr<Token>(new Token(Tag::ShiftRight)));
                else
                    throw SyntaxError("unexpected >?");
            }
            default:
                break;
        }
        /*
         * 处理进制数
         * 十六进制: 0xabcdef
         * 八进制:  0o1234567
         * 二进制:  0b1010101
         */
        if (c == '0') {
            // 处理小数
            if (reader.peek() == '.') goto __integer_float_number_state;
            // 整数第一个数不能为0
            if (isdigit(reader.peek()))
                throw ZeroNumberException(reader.peek() - '0');

            c = reader.get();
            string str;
            // 将非十进制的数字转化为十进制整数
            shared_ptr<Number> num = nullptr;

            // 判断下一个字符是否表示进制
            switch (c) {
                case 'x': {
                    c = reader.get();
                    while (ishex(c)) {
                        str.append(1, c);
                        c = reader.get();
                    }
                    if (isletter(c) && (c > 'f' || c > 'F'))
                        throw HexBinOctException(string(1, c));

                    num = shared_ptr<Number>(
                        new Number(toBase<Integer, 16>(str)));
                } break;
                case 'o': {
                    c = reader.get();
                    while (isoct(c)) {
                        str.append(1, c);
                        c = reader.get();
                    }
                    if ((isdigit(c) && (c > '7' && c <= '9')) || isletter(c))
                        throw HexBinOctException(string(1, c));
                    num =
                        shared_ptr<Number>(new Number(toBase<Integer, 8>(str)));
                } break;
                case 'b':
                    c = reader.get();
                    while (isbin(c)) {
                        str.append(1, c);
                        c = reader.get();
                    }
                    if ((isdigit(c) && (c > '1' && c <= '9')) || isletter(c))
                        throw HexBinOctException(string(1, c));
                    num =
                        shared_ptr<Number>(new Number(toBase<Integer, 2>(str)));
                    break;
                default:
                    // 单独的整数0
                    num = shared_ptr<Number>(new Number(0));
            }
            // 回退一个字符
            reader.back();
            return tokenlist.push_back(num);
        }

    __integer_float_number_state:
        // 整数或浮点数
        if (isdigit(c) || c == '.') {
            string s;
            bool once_add_sub = false, once_exponent = false;
            if (minus) s += '-';
            if (c == '.') goto __float_state;
            do {
                if (isexponent(c)) {
                    if (once_exponent)
                        throw SyntaxError("expoent e/E is to many!");
                    once_exponent = true;
                }
                // e/E+/-/数字
                if (c == '-' || c == '+') {
                    // 只能出现一次e/E+/-
                    if (isexponent(reader.backc())) {
                        once_add_sub = true;
                        goto __float_state;
                    } else
                        break;
                }
                s.append(1, c);
                c = reader.get();
            } while (isdigit(c) || isexponent(c) || c == '-' || c == '+');
            if (c != '.') {
                // 读取到最后一个字符，但是数字后面存在一个E
                if (c == EOF) {
                    if (once_exponent && !isdigit(reader.backc()))
                        throw SyntaxError("after digit e/E errro");
                }
                reader.back();
                lookforward = reader.cur();
                return tokenlist.push_back(
                    shared_ptr<Token>(new Number(toAny<double>(s))));
            }
        __float_state:
            // 添加小数点
            s.append(1, c);
            for (;;) {
                c = reader.get();
                if (isexponent(c)) {
                    if (once_exponent)
                        throw SyntaxError("expoent e/E is to many!");
                    once_exponent = true;
                }
                if (c == '+' || c == '-') {
                    // 再次出现 +/- 时就表示加减法
                    if (once_add_sub) break;
                    once_add_sub = true;
                    // 只能在e/E后面出现一次+/-
                    if (!isexponent(reader.backc())) break;
                }
                if (!isdigit(c) && !isexponent(c) && c != '+' && c != '-')
                    break;
                s.append(1, c);
            }
            reader.back();
            lookforward = reader.cur();
            return tokenlist.push_back(
                shared_ptr<Token>(new Float(toAny<double>(s))));
        }
        // 变量名/函数名 可以是字母和数字的组合
        if (isletter(c)) {
            string b;
            // 字母
            do {
                b.append(1, c);
                c = reader.get();
            } while (isletter(c));
            // 数字
            while (isdigit(c)) {
                b.append(1, c);
                c = reader.get();
            }
            // 如果变量名表示的是一个函数名(查表)
            if (c == '(') {
                if (isUnaryFunction(b)) {
                    reader.back();
                    is_function = true;
                    return tokenlist.push_back(
                        shared_ptr<Token>(new Function(b, minus)));
                } else if (isBinaryFunction(b)) {
                    reader.back();
                    is_function = true;
                    return tokenlist.push_back(
                        shared_ptr<Token>(new BinaryFunction(b, minus)));
                } else
                    throw FunctionNotDefined(b);
            } else {
                // 回退一个位置，因为还需要将 (
                // 作为token保存下来(目的是后续判断当前变量名是否是一个函数名)
                reader.back();
                // 变量名不存在，则保存到变量表中。注意，函数名不需要保存到表中！！！
                if (auto it = variable.find(b); it == variable.end()) {
                    auto word = shared_ptr<Token>(new Word(b, minus));
                    variable[b] = word;
                    return tokenlist.push_back(word);
                } else {
                    // 变量/常量附带一个负号标志
                    // 比如 a=100;b=-a / -pi
                    // 这里处理方法是将 -a 看作 -1 * a
                    if (minus) {
                        tokenlist.push_back(shared_ptr<Token>(new Number(-1)));
                        tokenlist.push_back(
                            shared_ptr<Token>(new Token(Tag::Mul)));
                    }
                    // 如果变量存在常量表中，那么就直接将这个变量替换为对应的常量值
                    if (auto x = constantTable.find(it->first);
                        x != constantTable.end()) {
                        return tokenlist.push_back(
                            shared_ptr<Token>(new Float(x->second)));
                    }
                    // 否则直接返回表中变量名
                    return tokenlist.push_back(it->second);
                }
            }
            // 变量声明的分隔符;
        } else if (c == ';') {
            // ; ; 过滤多个连续的分隔符
            if (!tokenlist.empty() && tokenlist.back()->type() == Tag::END_SEP)
                return;
            return tokenlist.push_back(
                shared_ptr<Token>(new Token(Tag::END_SEP)));

        } else if (c == '(') {
            // 函数的开始标志符 (
            if (is_function) {
                bracket_match.push(true);
                is_function = false;
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::BEGIN_FUNC)));
            }
            // 普通的左括号
            bracket_match.push(false);
            is_function = false;
            return tokenlist.push_back(
                shared_ptr<Token>(new Token(Tag::BEGIN_BRACKET)));

        } else if (c == ')') {
            // 如果栈为空，说明括号不匹配
            if (bracket_match.empty())
                throw SyntaxError("expression unexpected (!");

            // 函数的结尾标识符 )
            bool isEndOfFunction = bracket_match.top();
            bracket_match.pop();
            if (isEndOfFunction) {
                is_function = false;
                return tokenlist.push_back(
                    shared_ptr<Token>(new Token(Tag::END_FUNC)));
            }
            // 普通的右括号
            is_function = false;
            return tokenlist.push_back(
                shared_ptr<Token>(new Token(Tag::END_BRACKET)));
        }
        // 根据其字符来指定其token，比如二元函数的分隔符号,
        return tokenlist.push_back(shared_ptr<Token>(new Token(c)));
    }

    Reader reader;
    // token列表
    vector<shared_ptr<Token>> tokenlist;
    // 用于识别函数的声明定义左右括号的位置
    // 同时还可以检测表达式中括号是否匹配(栈不为空)
    stack<bool> bracket_match;

    // 是否是一元函数
    bool isUnaryFunction(const string& func) {
        return (unary_functions.find(func) != unary_functions.end());
    }
    // 是否是二元函数
    bool isBinaryFunction(const string& func) {
        return (binary_functions.find(func) != binary_functions.end());
    }
    // 保存变量名
    void putWord(const string& id) {
        if (variable.find(id) == variable.end())
            variable[id] = shared_ptr<Token>(new Word(id));
    }
    // 原始的常量名+用户定义的变量名
    void putConstant(const string& key, double value) {
        constantTable[key] = value;
        putWord(key);
    }

   protected:
    // 一些辅助函数
    bool ishex(char c) {
        return (c >= '0' && c <= 'f') || (c >= '0' && c <= 'F');
    }
    bool isdigit(char c) { return c >= '0' && c <= '9'; }
    bool isbin(char c) { return c == '0' || c == '1'; }
    bool isoct(char c) { return c >= '0' && c <= '7'; }

    bool isexponent(char c) { return c == 'e' || c == 'E'; }
    bool isletter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    bool isnewline(char c) { return c == '\n'; }
    bool isspace(char c) {
        return c == '\0' || c == '\t' || c == '\v' || c == ' ';
    }

   private:
    int line;
    char lookforward;
    bool is_function;
};