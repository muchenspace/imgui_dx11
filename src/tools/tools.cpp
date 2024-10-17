#include <stack>
#include "public.h"

import tools;



ImVec4 RGBAtoIV4(int r, int g, int b, int a)
{
    float newr = r / 255.0f;
    float newg = g / 255.0f;
    float newb = b / 255.0f;
    float newa = a / 255.0f;
    return ImVec4(newr, newg, newb, newa);
}



double priority(char op)
{
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

double applyOp(double a, double b, char op)
{
    using namespace std;
    switch (op)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
}
double evaluate(std::string tokens)
{
    using namespace std;
    int i;
    stack <double> values;
    stack <char> ops;
    for (i = 0; i < tokens.length(); i++)
    {
        if (tokens[i] == ' ')
            continue;
        else if (tokens[i] == '(')
        {
            ops.push(tokens[i]);
        }
        else if (isdigit(tokens[i]) || tokens[i] == '.')
        {
            double val = 0;
            double decimal = 1;
            bool isDecimal = false;
            while (i < tokens.length() && (isdigit(tokens[i]) || tokens[i] == '.'))
            {
                if (tokens[i] == '.')
                {
                    isDecimal = true;
                }
                else
                {
                    val = val * 10 + (tokens[i] - '0');
                    if (isDecimal)
                        decimal *= 10;
                }
                i++;
            }
            values.push(val / decimal);
            i--;
        }
        else if (tokens[i] == ')')
        {
            while (!ops.empty() && ops.top() != '(')
            {
                double val2 = values.top();
                values.pop();
                double val1 = values.top();
                values.pop();
                char op = ops.top();
                ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            if (!ops.empty())
                ops.pop();
        }
        else
        {
            while (!ops.empty() && priority(ops.top()) >= priority(tokens[i]))
            {
                double val2 = values.top();
                values.pop();
                double val1 = values.top();
                values.pop();
                char op = ops.top();
                ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(tokens[i]);
        }
    }
    while (!ops.empty()) {
        double val2 = values.top();
        values.pop();
        double val1 = values.top();
        values.pop();
        char op = ops.top();
        ops.pop();
        values.push(applyOp(val1, val2, op));
    }
    return values.top();
}


std::string WideCharToMultiByte(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}