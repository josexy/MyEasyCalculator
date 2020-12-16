#pragma once
#include <exception>
#include <string>

using namespace std;

class SyntaxError : public exception {
   public:
    SyntaxError(const string& error = "") { error_msg = "Error: " + error; }
    const char* what() const noexcept { return error_msg.c_str(); }

   protected:
    string error_msg;
};

// 函数未定义
class FunctionNotDefined : public SyntaxError {
   public:
    FunctionNotDefined(const string& function) {
        error_msg = "Error: function [" + function + "] not defined!";
    }
};

// 变量未定义
class VariableNotDefined : public SyntaxError {
   public:
    VariableNotDefined(const string& function) {
        error_msg = "Error: variable [" + function + "] not defined!";
    }
};

// 函数声明错误
class FunctionDeclareException : public SyntaxError {
   public:
    FunctionDeclareException(const string& function) {
        error_msg = "Error: function [" + function + "] unexpected ( ";
    }
};

// 函数没有)右括号
class FunctionClosureException : public SyntaxError {
   public:
    FunctionClosureException(const string& function) {
        error_msg = "Error: function [" + function + "] unexpected ) ";
    }
};

class ValueException : public SyntaxError {
   public:
    ValueException() {}
};

// 进制数转换异常
class HexBinOctException : public ValueException {
   public:
    HexBinOctException(const string& s) {
        error_msg = "Error: hex/oct/bin invalid [" + s + "]";
    }
};

// 除0异常
class DivZeroException : public ValueException {
   public:
    DivZeroException(double x, int y) {
        error_msg =
            "Error: div zero/0 error: " + to_string(x) + "/" + to_string(y);
    }
};

// 前导数字0
class ZeroNumberException : public ValueException {
   public:
    ZeroNumberException(int x) {
        error_msg = "Error: zero/0 number error: 0" + to_string(x);
    }
};

class NegateTypeException : public ValueException {
   public:
    NegateTypeException(double x) {
        error_msg = "Error: can not negate double/float type: " + to_string(x);
    }
};

// 左移和右移右操作数不能是负数
class ShiftNegativeException : public ValueException {
   public:
    ShiftNegativeException() {
        error_msg = "Error: shift left/right negative count";
    }
};
// 左移和右移操作数不能是浮点数
class ShiftLeftRightException : public ValueException {
   public:
    ShiftLeftRightException() {
        error_msg = "Error: can not shift left/right double/float type: ";
    }
};

class UnaryFunctionException : public ValueException {
   public:
    UnaryFunctionException(const string& functionname) {
        error_msg = "Error: unary function [" + functionname +
                    "] must have an operator number ";
    }
};
class BinaryFunctionException : public ValueException {
   public:
    BinaryFunctionException(const string& functionname) {
        error_msg = "Error: binary function [" + functionname +
                    "] must have two operator numbers: ";
    }
};
// 变量声明和定义需要;分隔
class DeclareVariableException : public SyntaxError {
   public:
    DeclareVariableException(const string& variable) {
        error_msg =
            "Error: declare variabe [" + variable + "] must be separated by ;";
    }
};

// 变量赋值需要=
class AssignVariableException : public SyntaxError {
   public:
    AssignVariableException(const string& variable) {
        error_msg =
            "Error: declare variabe [" + variable + "] must be separated by ;";
    }
};

// 还有其他的异常未添加...... :(