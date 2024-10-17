module;
#include "public.h"
#include <vector>
#include <filesystem>
#include <locale>
#include <codecvt>

export module widget;

import file;

export class widget
{
public:
	widget();
	bool ImageButton(std::string text, ImTextureID texture, ImVec2 size = ImVec2(0, 0));// ImageButton方法创建一个带有文本的图像按钮
	void CheckBox(std::string text, bool* b);// Checkbox函数创建一个复选框。参数b是一个指向bool的指针，用于存储复选框的状态，注意：调用者负责管理b指向的内存
	void TextView(std::string lable,std::string text,int width=200);//创建一个文本视图
	std::string StringList(const std::vector<std::string>& infos);//将传进的string列出，如果某一个string被点击就会返回这个string
	std::string FileList(std::filesystem::path path);
	void Speedometer(const int& speed, const ImVec2& center, const int& radius, ImColor Color = ImColor{0,255,0});
private:
	ImGuiStyle& style;
private:
	static void openFile(std::string path);
};