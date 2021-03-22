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
