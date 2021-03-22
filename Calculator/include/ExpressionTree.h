#ifndef MYEASYCALCULATOR_EXPRESSIONTREE_H
#define MYEASYCALCULATOR_EXPRESSIONTREE_H

#include <algorithm>
#include <numeric>
#include <vector>

#include "Lexer.h"

namespace calculator {

struct node {
    // 节点类型
    Tag type;
    // 当type表示一个函数时，funcname对应函数名，其他情况为空
    std::string funcname;
    // 当type表示一个函数时，negative表示其函数外是否有前导负号-
    bool negative;
    // 节点的值，根据孩子节点来计算
    double value;

    node *left;
    node *right;

    node(Tag t, double v, node *l = nullptr, node *r = nullptr)
        : type(t), value(v), left(l), right(r) {}
    node(Tag t) : node(t, 0.0) {}
};

class ExpressionTree {
   public:
    ExpressionTree() : root_(nullptr) { lexer_.tokenList().clear(); }
    explicit ExpressionTree(const std::string &text) : lexer_(text) {
        ExpressionTree();
    }

    ~ExpressionTree() { clear(root_); };

    double calcExpression(const std::string &text);

    // 添加变量
    void addVariable(const std::string &name, double value) {
        lexer_.putConstant(name, value);
    }
    // 添加一元函数
    void addUnaryFunction(const std::string &function_name,
                          const UnaryFunctionType &func) {
        lexer_.unary_functions[function_name] = std::move(func);
    }
    // 添加二元函数
    void addBinaryFunction(const std::string &function_name,
                           const BinaryFunctionType &func) {
        lexer_.binary_functions[function_name] = std::move(func);
    }

   private:
    void parseExpression(const std::string &text);
    node *buildTree();
    // 递归计算表达式树的值
    double calcValue(node *x);

    // token序列,中缀表达式构建语法分析树
    node *buildTreeInfix(int &token_index);
    void clear(node *&x);
    // 一元函数的计算
    double calcFunctionValue(node *x, std::string function);
    // 二元函数的计算
    double calcBinaryFunctionValuie(node *x, node *y, std::string function);
    // 一元操作数的计算
    Integer calcValue(node *x, Tag tag);
    // 二元操作数的计算
    double calcValue(node *x, node *y, Tag tag);
    // 操作符优先级
    int getPriority(char c, Tag tag);

   private:
    Lexer lexer_;
    Reader reader_;
    node *root_;
};
}  // namespace calculator
#endif