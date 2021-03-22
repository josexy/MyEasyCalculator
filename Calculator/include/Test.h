#ifndef MYEASYCALCULATOR_TEST_H
#define MYEASYCALCULATOR_TEST_H

#include <iomanip>
#include <iostream>

#include "ExpressionTree.h"
using namespace calculator;
using namespace std;

int gcd(int a, int b) {
    int maxa = a > b ? a : b;
    int minb = a < b ? a : b;
    if (minb == 0) return maxa;
    return gcd(minb, maxa % minb);
}

string expression;
void Test() {
    try {
        ExpressionTree et;
        et.addVariable("var", 999999);
        et.addUnaryFunction(
            "func", function<double(double)>([](double x) { return 2 * x; }));
        et.addBinaryFunction(
            "h", [](double x, double y) { return x * 10000 + y * 2000; });

        cout << "=> " << setprecision(10) << fixed
             << et.calcExpression(expression) << "\n";

    } catch (SyntaxError &e) {
        cerr << e.what() << endl;
    }
}

#define __TEST__ Test();

int expression_test() {
    expression =
        "100-sin(1234+10/18*cos(129))/1023*19999";  // 81.84392659975263
    __TEST__
    expression = "10-9+12*3/pow(2,10)-10*192";  // -1918.96484375
    __TEST__
    expression = "100-cos(17)+pow(3,8)";  // 6661.275163338051
    __TEST__
    expression =
        "a=100;b=1.234;c=0.234;-123*9/23.4+pow(c+a/b,3)-pow(cos(pi/"
        "3),sin(18332))*100/12*73.4324-192+192/992/cos(12)*pow(111,1e-3)*a/pi/"
        "(8*12/10/e)";  // 535572.732147942
    __TEST__
    expression = "exp(10)";  // 22026.465794806718
    __TEST__
    expression = "func(100)-sin(cos(pow(12,5)))";  // 199.47680614214548
    __TEST__
    expression =
        "a=100;b=1010100;a+1000+cos(a+pow(tan(a+b),2))/log(999/a)+cos(a)";  // 1100.4561295864257
    __TEST__
    expression = "e3=12345;log2(e3)";  // 13.591639216030144
    __TEST__
    expression = "h(-13e-4,-5)/log(100000)";  // -869.7181294594521
    __TEST__
    expression = "123.33*2";  // 246.66
    __TEST__
    expression = "19199&(172121|1910)^123";  // 516
    __TEST__
    expression = "2*2<<11>>1";  // 4096
    __TEST__
    expression = "a12=100;1+3*a12/10";  // 31.0
    __TEST__
    expression = "9**-3/12";  // 0.00011431184270690443
    __TEST__
    expression = "-311>>2";  // -78
    __TEST__
    expression = "pow(10<<2,20)";  // 1.099511627776e+32
    __TEST__
    expression = "10+0x11 + 100";  // 127
    __TEST__
    expression = "0b10101010";  // 170
    __TEST__
    expression = "0xffffeeAA / 0b0101010 + 0o7777 - 100";  // 102265015.42857143
    __TEST__
    expression = "0o10111 + 111.1234";  // 4280.1234
    __TEST__
    expression = "0. + 0.12121 + 0b10101010";  // 170.12121
    __TEST__
    expression = "1.001+0.-100";  // -98.999
    __TEST__
    expression = "a=pi;b=a;a+10.1+var/1000;";  // 1013.2405926535898
    __TEST__
    expression = "5/func(111)+12";  // 12.022522522522523
    __TEST__
    expression = "a=10;b=100*200;b/100.2";  // 199.6007984031936
    __TEST__
    expression = "a=10;x=-1000;b=100*pow(100,2)+100;b/2*x/a";  // -50005000.0
    __TEST__
    expression =
        "b=pi*e/(sin(tan(10)))/(1+-100*1.2/cos(0.12e4));a=b+122;a*10";  // 1218.81632320905
    __TEST__
    expression = "cos(12.34/(111.22*exp(3)))";  // 0.9999847430913299
    __TEST__
    expression = "sin((2+1))";  // 0.1411200080598672
    __TEST__
    expression =
        "a=1000 "
        ";b=0.341;a/0.1-100/1999*(a*b-111/23*123/0.12*a+b-a+b/(a+b*-123.33e3))";  // 257493.63629029953
    __TEST__
    expression = ";;;;y=10000*200;;;a=100;;;b=100;;;a+100+b*y;";  // 200000200
    __TEST__
    expression = "cos(2+(100)+100)";  // 0.591345375451585
    __TEST__
    expression = "cos(2+(100+2)+10)";  // 0.6195206125592099
    __TEST__
    expression =
        "a=10;0.0001e5*(pow(101*a/"
        "10,cos(0.31*pi*199)))-1*cos((2)+2)-max(cos(100),sin(200))/"
        "pow(2,cos(20))";  // 133.84679913329174
    __TEST__
    expression =
        "cos(((((((((((((((19))+10)))))/1000))))*1911*cos(100)))))";  // -0.7869416029408316
    __TEST__
    expression =
        "cos(2+(((((((((((((((((((((((((((((((((((((((((((sin(1111))))))))+100)"
        "))))*2000)))))))))*2)))))))))))))))))))))))";  // 0.16178461176298067
    __TEST__
    expression = "a=10<<2;a+1";  // 41
    __TEST__
    expression = "a=1000;x=a;b=x;b";  // 1000
    __TEST__
    expression =
        "-1000-log2(pi*e/-max(10,min(-100,-cos(-pi*22*exp(3)))))";  // -nan
    __TEST__
    expression = "a=100;b=-a-100;c=a-2*b/a;c";  // 104
    __TEST__
    expression = "1000-pow(100,2)";
    __TEST__
    expression =
        "-1000+-log2(-pi*e/-max(10,min(-100,-cos(pi*22*exp(3)))))";  // -999.7722630754739
    __TEST__
    expression = "-pi";  // -3.14......
    __TEST__
    expression = "-1+-log(101010)";  // -12.52297480082323
    __TEST__
    expression = " 1. - - - - -100";  // bad example
    __TEST__
    expression = "1. - - - - - -100";  // bad example
    __TEST__
    expression = "1. - - - - --100";  // bad example
    __TEST__
    expression = "1- - - - - -100";  // bad example
    __TEST__
    expression = "1*-pow(2,3)";  // -8
    __TEST__
    expression = "1--100";  // bad example
    __TEST__
    expression = "1-+100";  // -99
    __TEST__
    expression = "1+-100";  // -99
    __TEST__
    expression = "1++100";  // bad example
    __TEST__
    expression = "1 + -100 + 100 - +1000 - +1000 + -10100";  // -12099
    __TEST__
    expression = "-12e-3+12.33e+2-10.e1";  // -12e-3+12.33e+2-10.e1
    __TEST__
    expression = "100e3-100*pow(4,7)";  // -1538400.0
    __TEST__
    return 0;
}

#endif