module;

#include "public.h"

export module widget;

export class widget
{
public:
	widget();
	bool ImageButton(std::string text, ImTextureID texture, ImVec2 size = ImVec2(0, 0));// ImageButton��������һ�������ı���ͼ��ť
	void CheckBox(std::string text, bool* b);// Checkbox��������һ����ѡ�򡣲���b��һ��ָ��bool��ָ�룬���ڴ洢��ѡ���״̬��ע�⣺�����߸������bָ����ڴ�
	void TextView(std::string lable,std::string text,int width=200);
private:
	ImGuiStyle& style;
};