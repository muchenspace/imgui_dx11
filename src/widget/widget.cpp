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
        return false;//�ж��Ƿ���Ҫ����
    const ImGuiID id = window->GetID(text.c_str());//��text����һ��id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text�Ĵ�С
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
    

    ImGui::ItemAdd(bb, id);//����item
    ImGui::ItemSize(bb);//���²���

    if (ImGui::IsItemActive())
    {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//����һ������
    }// �����²���û�е���

    

    if (ImGui::IsItemClicked())
    {
        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(0, 0, 0));//����һ������
        return true;
    }
    return false;
}

void widget::checkbox(std::string text, bool* b)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;//�ж��Ƿ���Ҫ����


    const ImGuiID id = window->GetID(text.c_str());//��text����һ��id
    const ImVec2 text_size = ImGui::CalcTextSize(text.c_str(), NULL, true);//text�Ĵ�С
    const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50));//�ؼ���λ��

    window->DrawList->AddRectFilled(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + 50, window->DC.CursorPos.y + 50), ImColor(0, 0, 0));//����һ������
    if (*b)
    {
        window->DrawList->AddRectFilled(ImVec2(window->DC.CursorPos.x + 10, window->DC.CursorPos.y + 10), ImVec2(window->DC.CursorPos.x + 40, window->DC.CursorPos.y + 40), ImColor(0, 255, 0));
    }//���*bΪ�棬����һ����Ƕ����
    window->DrawList->AddText(ImVec2(window->DC.CursorPos.x + 55, window->DC.CursorPos.y + 25 - (text_size.y / 2)), ImColor(0, 255, 0), text.c_str());//��������
    ImGui::ItemAdd(bb, id);//����item
    ImGui::ItemSize(bb);//���²���
    if (ImGui::IsItemClicked())
    {
        *b = !*b;
    }//�����������ͷ�ת*b
}