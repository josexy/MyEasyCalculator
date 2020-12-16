#include <iostream>

#include "Calculator/ExpressionTree.h"
#include "Calculator/Test.h"

int main() {
    expression_test();
    ExpressionTree et;
    string line;
    while (cin.good()) {
        cout << ">>> ";
        getline(cin, line);
        if (line.empty()) continue;
        try {
            et.parseExpression(line);
            int j = 0;
            auto root = et.buildTreeInfix(j);
            if (root) {
                double x = et.calcValue(root);
                cout.precision(10);
                cout << fixed << "=> " << x << endl;
            }
        } catch (SyntaxError &e) {
            cout << e.what() << endl;
            cin.ignore();
            cin.clear();
        }
    }
    return 0;
}

