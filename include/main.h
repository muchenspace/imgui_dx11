#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Font.h"
#include <d3d11.h>
#include <stdio.h>
#include "imgui_internal.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <windows.h>
#include <stack>
#include <iostream>
#pragma comment(lib,"d3d11.lib")
#pragma execution_character_set("utf-8")

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

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
//算法来自百度



void myCheckbox(std::string text , bool* b)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;//判断是否需要绘制


    const ImGuiID id = window->GetID(text.c_str());//用text生成一个id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text的大小
    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50));//控件的位置

    window->DrawList->AddRectFilled(window->DC.CursorPos,ImVec2(window->DC.CursorPos.x+50, window->DC.CursorPos.y+50),ImColor(61,45,45));//添加一个背景
    if (*b)
    {
        window->DrawList->AddRectFilled(ImVec2(window->DC.CursorPos.x+10, window->DC.CursorPos.y + 10), ImVec2(window->DC.CursorPos.x + 40, window->DC.CursorPos.y + 40), ImColor(0, 255, 0));
    }//如果*b为真，添加一个内嵌矩形
    window->DrawList->AddText(ImVec2(window->DC.CursorPos.x + 55 , window->DC.CursorPos.y + 25 -(text_size.y/2)),ImColor(0,255,0),text.c_str());//添加文字
    ImGui::ItemAdd(bb, id);//添加item
    ImGui::ItemSize(bb);//更新布局
    if (ImGui::IsItemClicked())
    {
        *b = !*b;
    }//如果被点击，就反转*b
}


void myCheckbox2(std::string text,bool* b)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;//判断是否需要绘制

    ImGuiID id = window->GetID(text.c_str());
    ImRect bb(window->DC.CursorPos,ImVec2(window->DC.CursorPos.x+50, window->DC.CursorPos.y+50));
    ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text的大小

    window->DrawList->AddRectFilled(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50),ImColor(0,0,0));
    window->DrawList->AddText(ImVec2(window->DC.CursorPos.x + 55, window->DC.CursorPos.y + 25 - (text_size.y /2)), ImColor(0, 255, 0), text.c_str());
    if (*b)
    {
        window->DrawList->AddRectFilled(ImVec2(window->DC.CursorPos.x + 10, window->DC.CursorPos.y + 10), ImVec2(window->DC.CursorPos.x + 40, window->DC.CursorPos.y + 40), ImColor(0, 255, 0));
    }

    

    ImGui::ItemAdd(bb,id);
    ImGui::ItemSize(bb);

    if (ImGui::IsItemClicked())
    {
        *b = !*b;
    }
}