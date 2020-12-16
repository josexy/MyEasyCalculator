#pragma once

#include <math.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <vector>

#include "Lexer.h"
using namespace std;

struct node {
    Tag type;  // 节点类型
    string funcname;  // 当type表示一个函数时，funcname对应函数名，其他情况为空
    bool negative;  // 当type表示一个函数时，negative表示其函数外是否有前导负号-

    double value;  // 节点的值，根据孩子节点来计算

    node *left;
    node *right;

    node(Tag t, double v, node *l = nullptr, node *r = nullptr)
        : type(t), value(v), left(l), right(r) {}
    node(Tag t) : node(t, 0.0) {}
    ~node() {}
};

class ExpressionTree {
   public:
    ExpressionTree() { lexer.tokenlist.clear(); }

    ExpressionTree(const string &text) : lexer(text) {
        lexer.tokenlist.clear();
    }

    ~ExpressionTree() { clear(__root); };

    void parseExpression(const string &text) {
        // 重新设置文本串
        lexer.reader.set_buffer(text);
        // 清除token
        lexer.tokenlist.clear();
        // 词法分析阶段开始
        while (!lexer.reader.eof()) lexer.scan();

        // 然后判断表达式是否括号匹配
        if (!lexer.bracket_match.empty())
            throw SyntaxError("expression unexpected )!");
    }
    // 添加变量
    void addVariable(const string &name, double value) {
        lexer.putConstant(name, value);
    }
    // 添加一元函数
    void addUnaryFunction(const string &function_name,
                          function<double(double)> func) {
        lexer.unary_functions[function_name] = func;
    }
    // 添加二元函数
    void addBinaryFunction(const string &function_name,
                           function<double(double, double)> func) {
        lexer.binary_functions[function_name] = func;
    }
    void setDebug(bool is) { debug = is; }

    // 建表达式树
    node *buildTree() {
        int i = 0;
        __root = buildTreeInfix(i);
        return __root;
    }

    /**
     * @brief token序列,中缀表达式构建语法分析树
     * @note
     * @param  &token_index: token偏移
     * @retval
     */
    node *buildTreeInfix(int &token_index) {
        // 操作符栈
        stack<pair<string, Tag>> ops;
        // 操作数栈
        stack<node *> nodes;
        int i;
        for (i = token_index; i < lexer.tokenlist.size(); i++) {
            Token *token = lexer.tokenlist[i].get();
            // 函数的右闭括号 或者 二元函数的自变量分割符, 或者
            // 变量定义的结束分隔符;
            // 对 f(x) 和 f(x,y) 和 表达式赋值 a=1+2+cos(100);
            if (token->type() == Tag::END_FUNC || token->toString() == ",") {
                // 转到函数后面的token
                break;
            }
            // i!=0 防止第一个token是;
            if (i != 0 && token->type() == Tag::END_SEP) {
                break;
            }

            if (debug) cout << token->toString() << endl;

            string c = token->toString();
            // 一般括号，不是函数的声明
            if (token->type() == Tag::BEGIN_BRACKET) {  // (
                ops.push({c, Tag::BEGIN_BRACKET});
            } else if (token->type() == Tag::END_BRACKET) {  // )
                while (!ops.empty() &&
                       ops.top().second != Tag::BEGIN_BRACKET) {  // (
                    node *root = new node(ops.top().second);

                    // 一元操作符
                    if (ops.top().second == Tag::Negate ||
                        ops.top().second == Tag::Not) {
                        node *l = nodes.top();
                        nodes.pop();
                        node *root = new node(ops.top().second);
                        root->left = l;

                        nodes.push(root);
                        ops.pop();

                    } else {
                        // 二元操作符
                        node *r = nodes.top();
                        nodes.pop();
                        node *l = nodes.top();
                        nodes.pop();
                        root->left = l, root->right = r;

                        nodes.push(root);
                        ops.pop();
                    }
                }
                // 防止函数内部有多个 () 嵌套引发的异常
                if (!ops.empty()) {
                    // 删除多个 (
                    ops.pop();
                }
                continue;
                // 将数字添加到 操作数栈
            } else if (token->type() == Tag::Number ||
                       token->type() == Tag::Float) {
                node *x = nullptr;
                if (token->type() == Tag::Float)
                    x = new node(Tag::Float, ((Float *)token)->value());
                else
                    x = new node(Tag::Number, ((Number *)token)->value());
                nodes.push(x);

            } else if (token->type() == Tag::Identifier) {
                // 变量名/函数名
                string key = ((Word *)token)->lexeme();
                // 定义变量
                if (lexer.constantTable.find(key) ==
                    lexer.constantTable.end()) {
                    // 如果不是赋值，说明不是声明变量
                    if (i + 1 < lexer.tokenlist.size() &&
                        lexer.tokenlist[i + 1]->type() == Tag::Equal) {
                        // 跳到变量定义的部分，获取其值
                        i += 2;
                        // 如果当前的变量声明不是以;结束，则抛出异常
                        // 如果表达式中只有变量定义语句，即a=100此时;分隔符不是必须的
                        // a=100+200*cos(10);
                        // 否则进入到这里继续处理，直到遇到一个以;结尾表示变量定义结束
                        if (i + 1 < lexer.tokenlist.size() &&
                            lexer.tokenlist[i + 1]->type() != Tag::END_SEP) {
                            // 构建子表达式树,然后在计算这颗树的数值,保存到常量表中
                            auto node = buildTreeInfix(i);
                            lexer.constantTable[key] = calcValue(node);
                            // 释放子树内存，因为我们只需要这个子表达式的值
                            clear(node);
                            // 继续处理下一个token
                            continue;
                        }
                        // 定义的变量只有一个单值且以;结尾，如: a=100;
                        // 变量定义的值可以是:
                        // 1.整数/浮点数
                        // 2.已经定义的常量(在词法分析阶段已经被替换为对应的数值)/变量名所表示的数值
                        token = lexer.tokenlist[i].get();
                        if (token->type() == Tag::Number) {
                            lexer.constantTable[key] =
                                ((Number *)token)->value();
                        } else if (token->type() == Tag::Float) {
                            lexer.constantTable[key] =
                                ((Float *)token)->value();
                        } else if (token->type() == Tag::Identifier) {
                            // 然后再判断这个变量是否已经声明
                            if (auto x =
                                    lexer.constantTable.find(token->toString());
                                x != lexer.constantTable.end()) {
                                // 这里将变量b设置为a变量对应的值
                                lexer.constantTable[key] = x->second;

                            } else {
                                throw VariableNotDefined(token->toString());
                            }
                        }
                        // 跳过变量定义的分隔符 ; token
                        i++;
                    } else {
                        throw AssignVariableException(key);
                    }
                } else {
                    // 如果变量已经有值了,再次赋值时不会变化
                    nodes.push(new node(Tag::Float, lexer.constantTable[key]));
                }

                // 一元函数 f(x)
            } else if (token->type() == Tag::Function) {
                if (i + 1 >= lexer.tokenlist.size())
                    throw FunctionDeclareException(token->toString());
                // 缺少 (
                if (i + 1 < lexer.tokenlist.size() &&
                    lexer.tokenlist[i + 1]->type() != Tag::BEGIN_FUNC) {
                    throw FunctionDeclareException(c);
                } else {
                    // i+=2 的目的是跳过当前的左括号，直接来到自变量部分
                    i += 2;
                    // 缺少 )
                    if (i >= lexer.tokenlist.size()) {
                        throw FunctionClosureException(token->toString());
                    };
                    Token *nxToken = lexer.tokenlist[i].get();
                    // cos(1,) 或 cos() 或 cos(,) 的情况是不允许的
                    if (i + 1 >= lexer.tokenlist.size())
                        throw FunctionClosureException(token->toString());

                    // cos() 和 cos(,)
                    if (nxToken->type() == Tag::END_FUNC ||
                        (nxToken->toString() == "," &&
                         lexer.tokenlist[i + 1]->type() == Tag::END_FUNC))
                        throw UnaryFunctionException(token->toString());

                    node *root = new node(Tag::Function);
                    root->funcname = ((Function *)token)->lexeme();
                    root->negative = ((Function *)token)->negative();
                    root->left = buildTreeInfix(i);
                    // 缺少 )
                    if (i < lexer.tokenlist.size() &&
                        lexer.tokenlist[i]->type() != Tag::END_FUNC) {
                        throw FunctionClosureException(token->toString());
                    }
                    nodes.push(root);
                }
                // 二元函数 f(x,y)
            } else if (token->type() == Tag::BinaryFunction) {
                if (i + 1 >= lexer.tokenlist.size())
                    throw FunctionDeclareException(token->toString());
                if (i + 1 < lexer.tokenlist.size() &&
                    lexer.tokenlist[i + 1]->type() != Tag::BEGIN_FUNC) {
                    throw FunctionDeclareException(c);
                } else {
                    // 跳过左括号，进入自变量token
                    i += 2;
                    // 缺少 )
                    if (i >= lexer.tokenlist.size())
                        throw FunctionClosureException(token->toString());

                    node *root = new node(Tag::BinaryFunction);
                    root->funcname = ((BinaryFunction *)token)->lexeme();
                    root->negative = ((BinaryFunction *)token)->negative();
                    // 递归处理自变量X
                    root->left = buildTreeInfix(i);
                    // i+1 跳过 ,
                    int x = i + 1;
                    // 递归处理自变量Y
                    root->right = buildTreeInfix(x);
                    // 使当前token转移到 ) 然后继续处理下一个token
                    i = x;

                    // 缺少 )
                    if (i < lexer.tokenlist.size() &&
                        lexer.tokenlist[i]->type() != Tag::END_FUNC) {
                        throw FunctionClosureException(token->toString());
                    }
                    nodes.push(root);
                }
            } else {
                switch (token->type()) {
                    case Tag::Add:         // +
                    case Tag::Sub:         // -
                    case Tag::Mul:         // *
                    case Tag::Div:         // /
                    case Tag::And:         // &
                    case Tag::Or:          // |
                    case Tag::Xor:         // ^
                    case Tag::Not:         // !
                    case Tag::Negate:      // ~
                    case Tag::Mod:         // %
                    case Tag::ShiftLeft:   // <<
                    case Tag::ShiftRight:  // >>
                    case Tag::Pow:         // **
                    {
                        // 操作符栈顶的运算符优先级小于等于当前的操作符，取出操作数栈顶两个数字构建一个子表达式树，然后再重新添加到操作数栈
                        // 这里需要while循环来不断的取操作符，直到当前的操作符的优先级大于操作符栈顶的操作符
                        while (!ops.empty() &&
                               getPriority(c[0], token->type()) <=
                                   getPriority(ops.top().first[0],
                                               ops.top().second)) {
                            string c = ops.top().first;
                            // 一元运算符
                            if (ops.top().second == Tag::Negate ||
                                ops.top().second == Tag::Not) {
                                node *l = nodes.top();
                                nodes.pop();
                                node *root = new node(ops.top().second);

                                // 左节点
                                root->left = l;
                                nodes.push(root);
                                ops.pop();
                            }
                            // 二元运算符
                            else {
                                node *r = nodes.top();
                                nodes.pop();
                                node *l = nullptr;
                                if (!nodes.empty()) {
                                    l = nodes.top();
                                    nodes.pop();
                                }
                                node *root = new node(ops.top().second);
                                root->left = l, root->right = r;
                                nodes.push(root);
                                ops.pop();
                            }
                        }
                        // 当前读取到的操作符
                        ops.push({c, token->type()});
                    } break;
                    default:
                        break;
                }
            }
        }
        // token跳转
        token_index = i;

        // 每次从操作符栈取，直到空，此时表达式树构建完成
        while (!ops.empty()) {
            auto x = ops.top();
            node *l = nullptr, *r = nullptr;
            ops.pop();

            r = nodes.top();
            nodes.pop();
            // 对于一元运算符，只能取一个节点 ，这里将一元函数也看作是一元运算符
            if (x.second != Tag::Not && x.second != Tag::Negate &&
                x.second != Tag::Function) {
                if (!nodes.empty()) {
                    l = nodes.top();
                    nodes.pop();
                }
            }

            node *root = new node(x.second);

            if (x.second == Tag::Function || x.second == Tag::BinaryFunction)
                root->funcname = x.first;

            root->right = r, root->left = l;
            nodes.push(root);
        }
        // 表达式中没有计算式,只有变量定义
        if (nodes.empty()) return nullptr;

        auto x = nodes.top();
        nodes.pop();
        return x;
    }

    void clear(node *&x) {
        if (!x) return;
        clear(x->left);
        clear(x->right);
        delete x;
        x = nullptr;
    }

    // 一元函数的计算
    double calcFunctionValue(node *x, string function) {
        if (x == nullptr) throw UnaryFunctionException(function);
        if (auto it = lexer.unary_functions.find(function);
            it != lexer.unary_functions.end()) {
            return it->second(x->value);
        }
        throw FunctionDeclareException(function);
    }
    // 二元函数的计算
    double calcBinaryFunctionValuie(node *x, node *y, string function) {
        if (y == nullptr || x == nullptr)
            throw BinaryFunctionException(function);
        if (auto it = lexer.binary_functions.find(function);
            it != lexer.binary_functions.end()) {
            return it->second(x->value, y->value);
        }
        throw FunctionDeclareException(function);
    }
    // 一元操作数的计算
    Integer calcValue(node *x, Tag tag) {
        if (x == nullptr) throw SyntaxError("need one operator numbers");
        switch (tag) {
            case Tag::Not:
                return (Integer) !((Integer)x->value);
            case Tag::Negate:
                if (x->type != Tag::Number) {
                    throw NegateTypeException(x->value);
                }
                return ~((Integer)x->value);
            default:
                break;
        }
        return 0;
    }

    // 二元操作数的计算
    double calcValue(node *x, node *y, Tag tag) {
        if (y == nullptr || x == nullptr)
            throw SyntaxError("need two operator numbers");
        switch (tag) {
            case Tag::Add:
                return x->value + y->value;
            case Tag::Sub:
                return x->value - y->value;
            case Tag::Mul:
                return x->value * y->value;
            case Tag::Div:
                if (y->type == Tag::Number && y->value == 0) {
                    throw DivZeroException(x->value, y->value);
                }
                return x->value / y->value;
            case Tag::Mod:
                return fmod(x->value, y->value);
            case Tag::And:
                return (Integer)x->value & (Integer)y->value;
            case Tag::Or:
                return (Integer)x->value | (Integer)y->value;
            case Tag::Xor:
                return (Integer)x->value ^ (Integer)y->value;
            case Tag::ShiftLeft:
                // 左移或右移的操作数不能是浮点数
                if (x->type == Tag::Float || y->type == Tag::Float)
                    throw ShiftLeftRightException();
                // 左移或右移的右操作数不能是负数
                if (y->value < 0) throw ShiftNegativeException();
                return (Integer)x->value << (Integer)y->value;
            case Tag::ShiftRight:
                if (x->type == Tag::Float || y->type == Tag::Float)
                    throw ShiftLeftRightException();
                if (y->value < 0) throw ShiftNegativeException();
                return (Integer)x->value >> (Integer)y->value;
            case Tag::Pow:
                return lexer.binary_functions["pow"](x->value, y->value);
            default:
                break;
        }
        return 0;
    }

    // 递归计算表达式树的值
    double calcValue(node *x) {
        if (!x) return 0.0;

        // 更新操作符节点中的值
        auto l = calcValue(x->left);
        if (x->left) x->left->value = l;
        auto r = calcValue(x->right);
        if (x->right) x->right->value = r;

        node *valid_child = x->left ? x->left : x->right;
        if (x->type == Tag::Number || x->type == Tag::Float) return x->value;

        // 计算一元函数
        else if (x->type == Tag::Function) {
            auto val = calcFunctionValue(valid_child, x->funcname);
            if (x->negative) return -val;
            return val;
        }
        // 计算二元函数
        else if (x->type == Tag::BinaryFunction) {
            auto val = calcBinaryFunctionValuie(x->left, x->right, x->funcname);
            if (x->negative) return -val;
            return val;
        }
        // 计算一元操作符
        else if (x->type == Tag::Not || x->type == Tag::Negate)
            return (double)calcValue(valid_child, x->type);
        // 根据当前节点的tag 从孩子节点计算值 并存储到当前节点的 node.value 中
        return calcValue(x->left, x->right, x->type);
    }

#if 0
    // 前序遍历
    void preOrder(node *x) {
        if (!x) return;
        preOrder(x->left);
        preOrder(x->right);
    }
    // 中序遍历
    void inOrder(node *x) {
        if (!x) return;
        inOrder(x->left);
        inOrder(x->right);
    }
    // 后序遍历
    void postOrder(node *x) {
        if (!x) return;
        postOrder(x->left);
        postOrder(x->right);
    }
#endif
    // 操作符优先级
    int getPriority(char c, Tag tag) {
        // 函数的优先级最高,对于 ** 求指数幂，也可以看作函数
        if (tag == Tag::Function || tag == Tag::BinaryFunction ||
            tag == Tag::Pow)
            return 200;
        // 左移和右移运算优先级最低
        if (tag == Tag::ShiftLeft || tag == Tag::ShiftRight) return 0;

        int priority = 0;
        switch (c) {
            case '!':
            case '~':
                priority = 200;
                break;
            case '*':
            case '/':
            case '%':
            case '^':
            case '&':
            case '|':
                priority = 100;
                break;
            case '+':
            case '-':
                priority = 90;
                break;
            default:
                break;
        }
        return priority;
    }

   private:
    Lexer lexer;
    Reader reader;
    bool debug = false;
    node *__root = nullptr;
};