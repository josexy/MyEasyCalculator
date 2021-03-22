### MyEasyCalculator

最近在看《编译原理》——龙书，根据前面的一些知识设计了一个简单的计算器，目前实现的功能大致如下：

#### 已完成

- 支持 `+-*/` 基本运算
- 支持取模 `%` ，与或非取反异或 `&|!~^`
- 支持左移和右移 `<<,>>`
- 支持类似Python的指数幂 `**`
- 支持输入十六进制、八进制、二进制，前导标识符号分别为 `0x,0o,0b`
- 支持自定义数值**常量**和自定义**变量**，声明定义变量需要以 `;` 分隔
- 支持函数（一元和二元函数）来计算表达式，可以自定义函数（函数指针）
- 浮点数支持科学表示（e/E）
- 内置部分**常量和函数**
- 支持在计算表达式时引入之前定义的变量
- 支持复合函数使用（语法树分析）`sin(cos(100*pi))`
- 添加部分**异常**检测功能
- 支持将一个表达式结果赋值给变量
  比如：`a=2+3` 、`b=sin(pi/3)` 
- 对于函数内部有多个括号也可以识别出来，比如：`cos((((x))+100))`
- 支持对变量直接取负 `a=-b`
- 支持对函数直接取负 `-pow(100,2)`


#### 方法
对于一个输入的表达式，我们需要对其词法分析，得到一个token序列，然后再构建一颗语法表达式树，最后对这颗树递归计算值就能得到表达式的值。看起来虽然简单，但其中的细节有很多需要注意的。


#### Usage
```bash
git clone https://github.com/josexy/MyEasyCalculator.git
cd MyEasyCalculator
mkdir build && cd build
cmake ..
make
./calculator
```

#### Main
```cpp
#include <iostream>

#include "Calculator/include/ExpressionTree.h"
#include "Calculator/include/Test.h"
using namespace calculator;
using namespace std;

int main() {
    expression_test();
    ExpressionTree et;
    string line;
    while (cin.good()) {
        cout << ">>> ";
        getline(cin, line);
        if (line.empty()) continue;
        try {
            double x = et.calcExpression(line);
            cout.precision(10);
            cout << fixed << "=> " << x << endl;
        } catch (SyntaxError &e) {
            cout << e.what() << endl;
            cin.ignore();
            cin.clear();
        }
    }
    return 0;
}

```

#### 常量表

| 常量名 |    数值(浮点数)    |
| :----: | :----------------: |
|   pi   | 3.141592653589793  |
|   e    | 2.718281828459045  |
| sqrt2  | 1.4142135623730951 |

#### 一元函数

| 函数名 | 内部函数 |
| :----: | :------: |
|  sqrt  |  sqrtf   |
|  ceil  |  ceilf   |
|  sin   |   sinf   |
|  cos   |   cosf   |
|  tan   |   tanf   |
|  log   |   logf   |
| floor  |  floorf  |
|  acos  |  acosf   |
|  asin  |  asinf   |
|  atan  |  atanf   |
|  exp   |   expf   |
| log10  |  log10f  |
|  log2  |  log2f   |
| round  |  roundf  |
|  erf   |   erff   |

#### 二元函数

| 函数名 |   内部函数    |
| :----: | :-----------: |
|  pow   |     powf      |
|  max   | max（自定义） |
|  min   | min（自定义） |

#### 测试用例
以下列举的一些表达式在Python3中的测试结果与该计算器输出的结果基本大致相同(存在精度的差异)，当然前提是要保证输入的表达式正确。

```cpp
expression = "100-sin(1234+10/18*cos(129))/1023*19999";  // 81.84392659975263
expression = "10-9+12*3/pow(2,10)-10*192";  // -1918.96484375
expression = "100-cos(17)+pow(3,8)";  // 6661.275163338051
expression = "a=100;b=1.234;c=0.234;-123*9/23.4+pow(c+a/b,3)-pow(cos(pi/3),sin(18332))*100/12*73.4324-192+192/992/cos(12)*pow(111,1e-3)*a/pi/(8*12/10/e)";  // 535572.732147942
expression = "exp(10)";  // 22026.465794806718
expression = "a=100;b=1010100;a+1000+cos(a+pow(tan(a+b),2))/log(999/a)+cos(a)"; // 1100.4561295864257
```

