#include "public.h"
#include <vector>
#include <sstream>
import widget;

widget::widget() : style(GImGui->Style) 
{

}

bool widget::ImageButton(std::string text, ImTextureID texture, ImVec2 size)
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
        //window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//添加一个背景
    }// 被按下并且没有弹出

    

    if (ImGui::IsItemClicked())
    {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//添加一个背景
        return true;
    }
    return false;
}

void widget::CheckBox(std::string text, bool* b)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;//判断是否需要绘制



    const ImGuiID id = window->GetID(text.c_str());//用text生成一个id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text的大小
    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 30, window->DC.CursorPos.y + 30));//控件的位置

    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(style.Colors[ImGuiCol_FrameBg]));//添加一个背景
    if (*b)
    {
        window->DrawList->AddRectFilled(ImVec2( bb.Min.x + 5,bb.Min.y + 5 ),ImVec2( bb.Max.x -5,bb.Max.y -5 ), ImColor(style.Colors[ImGuiCol_CheckMark]));
    }//如果*b为真，添加一个内嵌矩形
    window->DrawList->AddText(ImVec2(bb.Max.x, bb.Max.y - 15 - (text_size.y / 2)), ImColor(style.Colors[ImGuiCol_Text]), text.c_str());//添加文字
    ImGui::ItemAdd(bb, id);//添加item
    ImGui::ItemSize(ImRect(bb.Min,ImVec2( bb.Max.x+text_size.x,bb.Max.y)));//更新布局,记得把文字的大小添加上，不然两个控件在一行会乱，别问我为什么不直接加到bb里，问就是不舒服（doge
    if (ImGui::IsItemClicked())
    {
        *b = !*b;
    }//如果被点击，就反转*b
}


void widget::TextView(std::string lable,std::string text, int width)
{
    ImGui::TextWrapped(lable.c_str());
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + width);
    ImGui::Text(text.c_str(), width);
    draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
    ImGui::PopTextWrapPos();
}

int widget::StringList(const std::vector<std::string>& infos)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return 0;//判断是否需要绘制

    for (auto& info : infos)
    {
        int pid{};
        const ImVec2 TextSize = ImGui::CalcTextSize(info.c_str(), NULL, true);//text的大小
        const ImGuiID id = window->GetID(info.c_str());//用text生成一个id
        const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + TextSize.x, window->DC.CursorPos.y + TextSize.y));//item的位置
        window->DrawList->AddText(window->DC.CursorPos, ImColor(style.Colors[ImGuiCol_Text]), info.c_str());
        ImGui::ItemAdd(bb, id);//添加item
        ImGui::ItemSize(ImRect(bb));//更新布局
        if (ImGui::IsItemClicked())
        {
            std::istringstream iss(info);
            iss >> pid;
            window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//添加一个背景
            return pid;
        }
    }
    return 0;
}
