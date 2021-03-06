#include "../include/Lexer.h"
using namespace calculator;

Lexer::Lexer() : line_(0), lookforward_(0), is_function_(false) {
    constant.clear();
    variable.clear();
    tokenlist_.clear();

    putConstant("pi", 3.141592653589793);
    putConstant("e", 2.718281828459045);
    putConstant("sqrt2", 1.4142135623730951);
}

void Lexer::scan() {
    char c;
    // 标志是否为负数
    bool minus = false;
    // 跳过空白字符
    while (!reader_.eof()) {
        c = reader_.get();
        if (c == EOF) return;
        if (isspace(c))
            continue;
        else if (isnewline_(c))
            line_++;
        else if (c == '+' || c == '-') {
            char nc;
            // 判断是否出现多个连续的 +-符号
            while (isspace((nc = reader_.get())))
                ;
            if (c == nc) {
                throw SyntaxError("can not present continue symbol ++/--");
            } else {
                reader_.back();
                break;
            }
        } else
            break;
    }

    // )+   )- 向前看字符如果是 ) 说明当前的 +/- 一定是一个 减法或加法
    if (lookforward_ != ')' && !isexponent(lookforward_) &&
        (c == '-' || c == '+')) {
        // +100 / -100  0.+100
        // 对于表达式 1 + -100 和 1. - 100 可以识别出来
        if (!isdigit(lookforward_) && !isletter(lookforward_) &&
            lookforward_ != '.') {
            if (c == '-') minus = true;
            c = reader_.get();
            while (isspace(c)) c = reader_.get();
        }
    }
    // 保存前一个字符
    lookforward_ = c;

    switch (c) {
        case '&':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::And)));
        case '|':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Or)));
        case '!':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Not)));
        case '^':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Xor)));
        case '~':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Negate)));
        case '+':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Add)));
        case '-': {
            // -表示一个负数，而非减法
            if (minus) break;
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Sub)));
        }
        case '*': {
            if (auto [ok, c] = reader_.geteq('*'); ok) {
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new Token(Tag::Pow)));
            } else {
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new Token(Tag::Mul)));
            }
        }
        case '/':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Div)));
        case '=':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Equal)));
        case '%':
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::Mod)));
        case '<': {
            // <<左移
            if (auto [ok, c] = reader_.geteq('<'); ok)
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new Token(Tag::ShiftLeft)));
            else
                throw SyntaxError("unexpected <?");
        } break;
        case '>': {
            // >>右移
            if (auto [ok, c] = reader_.geteq('>'); ok)
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new Token(Tag::ShiftRight)));
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
        if (reader_.peek() == '.') goto __integer_float_number_state;
        // 整数第一个数不能为0
        if (isdigit(reader_.peek()))
            throw ZeroNumberException(reader_.peek() - '0');

        c = reader_.get();
        std::string str;
        // 将非十进制的数字转化为十进制整数
        std::shared_ptr<Number> num = nullptr;

        // 判断下一个字符是否表示进制
        switch (c) {
            case 'x': {
                c = reader_.get();
                while (ishex(c)) {
                    str.append(1, c);
                    c = reader_.get();
                }
                if (isletter(c) && (c > 'f' || c > 'F'))
                    throw HexBinOctException(std::string(1, c));

                num = std::shared_ptr<Number>(
                    new Number(toBase<Integer, 16>(str)));
            } break;
            case 'o': {
                c = reader_.get();
                while (isoct(c)) {
                    str.append(1, c);
                    c = reader_.get();
                }
                if ((isdigit(c) && (c > '7' && c <= '9')) || isletter(c))
                    throw HexBinOctException(std::string(1, c));
                num = std::shared_ptr<Number>(
                    new Number(toBase<Integer, 8>(str)));
            } break;
            case 'b':
                c = reader_.get();
                while (isbin(c)) {
                    str.append(1, c);
                    c = reader_.get();
                }
                if ((isdigit(c) && (c > '1' && c <= '9')) || isletter(c))
                    throw HexBinOctException(std::string(1, c));
                num = std::shared_ptr<Number>(
                    new Number(toBase<Integer, 2>(str)));
                break;
            default:
                // 单独的整数0
                num = std::shared_ptr<Number>(new Number(0));
        }
        // 回退一个字符
        reader_.back();
        return tokenlist_.push_back(num);
    }

__integer_float_number_state:
    // 整数或浮点数
    if (isdigit(c) || c == '.') {
        std::string s;
        bool once_add_sub = false, once_exponent = false;
        if (minus) s += '-';
        if (c == '.') goto __float_state;
        do {
            if (isexponent(c)) {
                if (once_exponent) throw SyntaxError("expoent e/E is to many!");
                once_exponent = true;
            }
            // e/E+/-/数字
            if (c == '-' || c == '+') {
                // 只能出现一次e/E+/-
                if (isexponent(reader_.backc())) {
                    once_add_sub = true;
                    goto __float_state;
                } else
                    break;
            }
            s.append(1, c);
            c = reader_.get();
        } while (isdigit(c) || isexponent(c) || c == '-' || c == '+');
        if (c != '.') {
            // 读取到最后一个字符，但是数字后面存在一个E
            if (c == EOF) {
                if (once_exponent && !isdigit(reader_.backc()))
                    throw SyntaxError("after digit e/E errro");
            }
            reader_.back();
            lookforward_ = reader_.cur();
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Number(toAny<double>(s))));
        }
    __float_state:
        // 添加小数点
        s.append(1, c);
        for (;;) {
            c = reader_.get();
            if (isexponent(c)) {
                if (once_exponent) throw SyntaxError("expoent e/E is to many!");
                once_exponent = true;
            }
            if (c == '+' || c == '-') {
                // 再次出现 +/- 时就表示加减法
                if (once_add_sub) break;
                once_add_sub = true;
                // 只能在e/E后面出现一次+/-
                if (!isexponent(reader_.backc())) break;
            }
            if (!isdigit(c) && !isexponent(c) && c != '+' && c != '-') break;
            s.append(1, c);
        }
        reader_.back();
        lookforward_ = reader_.cur();
        return tokenlist_.push_back(
            std::shared_ptr<Token>(new Float(toAny<double>(s))));
    }
    // 变量名/函数名 可以是字母和数字的组合
    if (isletter(c)) {
        std::string b;
        // 字母
        do {
            b.append(1, c);
            c = reader_.get();
        } while (isletter(c));
        // 数字
        while (isdigit(c)) {
            b.append(1, c);
            c = reader_.get();
        }
        // 如果变量名表示的是一个函数名(查表)
        if (c == '(') {
            if (isUnaryFunction(b)) {
                reader_.back();
                is_function_ = true;
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new Function(b, minus)));
            } else if (isBinaryFunction(b)) {
                reader_.back();
                is_function_ = true;
                return tokenlist_.push_back(
                    std::shared_ptr<Token>(new BinaryFunction(b, minus)));
            } else
                throw FunctionNotDefined(b);
        } else {
            // 回退一个位置，因为还需要将 (
            // 作为token保存下来(目的是后续判断当前变量名是否是一个函数名)
            reader_.back();
            // 变量名不存在，则保存到变量表中。注意，函数名不需要保存到表中！！！
            if (auto it = variable.find(b); it == variable.end()) {
                auto word = std::shared_ptr<Token>(new Word(b, minus));
                variable[b] = word;
                return tokenlist_.push_back(word);
            } else {
                // 变量/常量附带一个负号标志
                // 比如 a=100;b=-a / -pi
                // 这里处理方法是将 -a 看作 -1 * a
                if (minus) {
                    tokenlist_.push_back(
                        std::shared_ptr<Token>(new Number(-1)));
                    tokenlist_.push_back(
                        std::shared_ptr<Token>(new Token(Tag::Mul)));
                }
                // 如果变量存在常量表中，那么就直接将这个变量替换为对应的常量值
                if (auto x = constant.find(it->first); x != constant.end()) {
                    return tokenlist_.push_back(
                        std::shared_ptr<Token>(new Float(x->second)));
                }
                // 否则直接返回表中变量名
                return tokenlist_.push_back(it->second);
            }
        }
        // 变量声明的分隔符;
    } else if (c == ';') {
        // ; ; 过滤多个连续的分隔符
        if (!tokenlist_.empty() && tokenlist_.back()->type() == Tag::END_SEP)
            return;
        return tokenlist_.push_back(
            std::shared_ptr<Token>(new Token(Tag::END_SEP)));

    } else if (c == '(') {
        // 函数的开始标志符 (
        if (is_function_) {
            bracket_match_.push(true);
            is_function_ = false;
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::BEGIN_FUNC)));
        }
        // 普通的左括号
        bracket_match_.push(false);
        is_function_ = false;
        return tokenlist_.push_back(
            std::shared_ptr<Token>(new Token(Tag::BEGIN_BRACKET)));

    } else if (c == ')') {
        // 如果栈为空，说明括号不匹配
        if (bracket_match_.empty())
            throw SyntaxError("expression unexpected (!");

        // 函数的结尾标识符 )
        bool isEndOfFunction = bracket_match_.top();
        bracket_match_.pop();
        if (isEndOfFunction) {
            is_function_ = false;
            return tokenlist_.push_back(
                std::shared_ptr<Token>(new Token(Tag::END_FUNC)));
        }
        // 普通的右括号
        is_function_ = false;
        return tokenlist_.push_back(
            std::shared_ptr<Token>(new Token(Tag::END_BRACKET)));
    }
    // 根据其字符来指定其token，比如二元函数的分隔符号,
    return tokenlist_.push_back(std::shared_ptr<Token>(new Token(c)));
}
