#ifndef widgeth
#define widgeth
#include "public.h"
class widget
{
public:
	widget();
	bool ImageButton(std::string text, ID3D11ShaderResourceView* texture, ImVec2 size = ImVec2(0, 0));// ImageButton��������һ�������ı���ͼ��ť
	void checkbox(std::string text, bool* b);// Checkbox��������һ����ѡ�򡣲���b��һ��ָ��bool��ָ�룬���ڴ洢��ѡ���״̬��ע�⣺�����߸������bָ����ڴ�
private:
	ImGuiStyle& style;
};
#endif