#include "widget.h"

widget::widget() : style(GImGui->Style) 
{

}

bool widget::ImageButton(std::string text, ID3D11ShaderResourceView* texture, ImVec2 size)
{
    if (texture == nullptr) 
    {
        return false;
    }

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;//判断是否需要绘制
    const ImGuiID id = window->GetID(text.c_str());//用text生成一个id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text的大小
    ImRect bb{};

    if (size.x == 0 || size.y == 0)
    {
       bb = ImRect (window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + text_size.x, window->DC.CursorPos.y + text_size.y));
       window->DrawList->AddImage(texture, bb.Min, bb.Max);
       window->DrawList->AddText(window->DC.CursorPos,ImColor(style.Colors[ImGuiCol_Text]),text.c_str());
    }
    else
    {
        bb = ImRect(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
        window->DrawList->AddImage(texture, bb.Min, bb.Max);
        window->DrawList->AddText(ImVec2(window->DC.CursorPos.x +(size.x/2) - (text_size.x/2), window->DC.CursorPos.y + (size.y / 2) - (text_size.y / 2)), ImColor(style.Colors[ImGuiCol_Text]), text.c_str());
    }
    

    ImGui::ItemAdd(bb, id);//添加item
    ImGui::ItemSize(bb);//更新布局

    if (ImGui::IsItemActive())
    {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//添加一个背景
    }// 被按下并且没有弹出

    

    if (ImGui::IsItemClicked())
    {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//添加一个背景
        return true;
    }
    return false;
}

void widget::checkbox(std::string text, bool* b)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;//判断是否需要绘制


    const ImGuiID id = window->GetID(text.c_str());//用text生成一个id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text的大小
    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50));//控件的位置

    window->DrawList->AddRectFilled(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50), ImColor(0, 0, 0));//添加一个背景
    if (*b)
    {
        window->DrawList->AddRectFilled(ImVec2(window->DC.CursorPos.x + 10, window->DC.CursorPos.y + 10), ImVec2(window->DC.CursorPos.x + 40, window->DC.CursorPos.y + 40), ImColor(0, 255, 0));
    }//如果*b为真，添加一个内嵌矩形
    window->DrawList->AddText(ImVec2(window->DC.CursorPos.x + 55, window->DC.CursorPos.y + 25 - (text_size.y / 2)), ImColor(0, 255, 0), text.c_str());//添加文字
    ImGui::ItemAdd(bb, id);//添加item
    ImGui::ItemSize(bb);//更新布局
    if (ImGui::IsItemClicked())
    {
        *b = !*b;
    }//如果被点击，就反转*b
}