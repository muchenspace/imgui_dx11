#include "main.h"
#include "public.h"
#include "Font.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>

#pragma comment(lib,"d3d11.lib")
#pragma execution_character_set("utf-8")


import tools;
import widget;
import file;
/*
						  _ooOoo_
						 o8888888o
						 88" . "88
						 (| -_- |)
						 O\  =  /O
					  ____/`---'\____
					.'  \\|     |//  `.
				   /  \\|||  :  |||//  \
				  /  _||||| -:- |||||-  \
				  |   | \\\  -  /// |   |
				  | \_|  ''\---/''  |   |
				  \  .-\__  `-`  ___/-. /
				___`. .'  /--.--\  `. . __
			 ."" '<  `.___\_<|>_/___.'  >'"".
			| | :  `- \`.;`\ _ /`;.`/ - ` : | |
			\  \ `-.   \_ __\ /__ _/   .-` /  /
	   ======`-.____`-.___\_____/___.-`____.-'======
						  `=---='
	   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
				佛祖保佑                  永无BUG
				佛祖镇楼                  BUG辟易
		   佛曰:
				   写字楼里写字间，写字间里程序员；
				   程序人员写程序，又拿程序换酒钱。
				   酒醒只在网上坐，酒醉还来网下眠；
				   酒醉酒醒日复日，网上网下年复年。
				   但愿老死电脑间，不愿鞠躬老板前；
				   奔驰宝马贵者趣，公交自行程序员。
				   别人笑我忒疯癫，我笑自己命太贱；
				   不见满街漂亮妹，哪个归得程序员？
 */


 //ImGui_ImplWin32_EnableAlphaCompositing(hwnd);//为渲染窗口设置透明
 //
 //LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
 //exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
 //SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);//鼠标穿透，记得创建新窗口的时候让窗口脱离，不然会一起没触摸

 //SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置优先级别最高

 //ImGuiWindowClass noAutoMerge;
 //noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
 //ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离渲染窗口
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource{};
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}




int main()
{

	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);





	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"imgui", WS_POPUP, 0, 0, 2560, 1440, nullptr, nullptr, wc.hInstance, nullptr);//隐藏标题
	//HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"imgui", WS_OVERLAPPEDWINDOW, 100,100, 800, 600, nullptr, nullptr, wc.hInstance, nullptr);




	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}


	::ShowWindow(hwnd, 10);
	::UpdateWindow(hwnd);

	ImGui_ImplWin32_EnableAlphaCompositing(hwnd);//窗口透明

	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
	SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);//鼠标穿透


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.ConfigViewportsNoAutoMerge = true;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	io.IniFilename = nullptr;
	ImFontConfig Font_cfg;
	Font_cfg.FontDataOwnedByAtlas = false;


	io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 20.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
	ImFont* Font_Big = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 60.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());


	//io.Fonts->AddFontFromFileTTF("C:/Users/admin/Downloads/1.ttf", 22.0f, nullptr,io.Fonts->GetGlyphRangesChineseFull());
	style.FrameRounding = 12;
	ImGui::StyleColorsLight();
	style.Colors[ImGuiCol_Header] = RGBAtoIV4(36, 54, 74, 79);

	int my_image_width1;//图长
	int my_image_height1;//图宽
	int jcly_image_width;//图长
	int jcly_image_height;//图宽
	ID3D11ShaderResourceView* my_texture = NULL;
	ID3D11ShaderResourceView* my_texture2 = NULL;
	ID3D11ShaderResourceView* my_texture3 = NULL;
	ID3D11ShaderResourceView* my_texture4 = NULL;
	bool ret = LoadTextureFromFile("1.png", &my_texture, &my_image_width1, &my_image_height1);
	bool ret2 = LoadTextureFromFile("2.png", &my_texture2, &my_image_width1, &my_image_height1);
	bool ret3 = LoadTextureFromFile("jcly.png", &my_texture3, &jcly_image_width, &jcly_image_height);
	bool ret4 = LoadTextureFromFile("wz2.png", &my_texture4, &my_image_width1, &my_image_height1);
	IM_ASSERT(ret);
	IM_ASSERT(ret2);
	IM_ASSERT(ret3);
	IM_ASSERT(ret4);




	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}

		if (done)
			break;


		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}


		//////////////////////////////////定义区
		static widget test{};
		static File filetest{ "1.txt" };
		static char buffer[999] = "";

		static bool show_demo_window{};
		static bool show_Speedometer{};
		static bool show_another_window{};
		static bool show_line{};
		static bool show_Circle{};
		static bool show_test{};
		static bool show_box{};
		static bool show_calculator{};

		static float line_cx = 1;
		static float Circle_cx = 2;
		static float box_cx = 1;
		static float arc_cx = 3;

		static float box_yj = 1;

		static int distance{ 13 };

		static float hp{ 82 };

		static ImVec2 bg_zr{ 444.5,19.5 };
		static ImVec2 bg_bot{ 573.1,19.5 };
		static ImVec2 line_coord1{ 576,82.3 };
		static ImVec2 line_coord2{ 640,314 };
		static ImVec2 box_coord1{ 584.7,340.1 };
		static ImVec2 box_coord2{ 702.3,547.7 };
		static ImVec2 Circle_coord{ 400,800 };


        static int speed{};

		static std::string result{};

		static int menutap{ 1 };

		static ImColor Circle_color = ImColor{ 255, 0, 0 };
		static ImVec4  clear_color = ImVec4{ 0, 0, 0, 0 };
		static ImColor line_color = ImColor{ 255, 255, 255 };
		static ImColor box_color = ImColor{ 0, 255, 0 };
		static ImGuiStyle ref_saved_style;
		static int style_idx = 0;
		//////////////////////////////////定义区结束





		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置优先级别最高
		if (show_Speedometer)
		{
			test.Speedometer(speed, { 800,800 }, 300);
		}
		if (show_Circle)
		{
			ImGui::GetForegroundDrawList()->AddCircle(Circle_coord, 100, Circle_color, 0, Circle_cx);
		}
		
		if (show_line)
		{
			ImGui::GetForegroundDrawList()->AddLine(line_coord1, line_coord2, line_color, line_cx);
		}
		if (show_box)
		{
			ImGui::GetForegroundDrawList()->AddRect(box_coord1, box_coord2, box_color, box_yj, NULL, box_cx);
		}
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow();
		}
		if (show_test)
		{
			ImGui::GetForegroundDrawList()->AddRect(box_coord1, box_coord2, box_color, box_yj, NULL, box_cx);
			ImGui::GetForegroundDrawList()->AddLine(line_coord1, line_coord2, line_color, line_cx);
			ImGui::GetForegroundDrawList()->PathArcTo(ImVec2{ 645,239 }, 82, 0.0f, hp / 15.9154f, 50);
			ImGui::GetForegroundDrawList()->PathStroke(ImColor(0, 255, 0), false, 3.0);
			ImGui::GetBackgroundDrawList()->AddImage(my_texture, bg_bot, ImVec2(bg_bot.x + 128, bg_bot.y + 64));
			ImGui::GetBackgroundDrawList()->AddImage(my_texture2, bg_zr, ImVec2(bg_zr.x + 128, bg_zr.y + 64));
			ImGui::GetBackgroundDrawList()->AddText(ImVec2{ 645.2,42.3 }, ImColor(0, 255, 0), "26");
			ImGui::GetBackgroundDrawList()->AddText(ImVec2{ 522.4,40.8 }, ImColor(555556666767660, 255, 0), "25");
			ImGui::GetBackgroundDrawList()->AddText(ImVec2(590.5, 320), ImColor(0, 255, 0), "[Ai]");
			ImGui::PushFont(Font_Big);
			ImGui::GetBackgroundDrawList()->AddText(ImVec2{ 578,205 }, ImColor(0, 255, 0), std::string(std::to_string(distance) + std::string(" m ")).c_str());
			ImGui::PopFont();
		}
		{
			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
			ImGui::Begin("测试");
			ImGui::SetWindowSize({ 600, 800 }, ImGuiCond_Once);
	
			test.FileList("D:/");

			ImGui::End();
		}
		{
			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
			ImGui::Begin("竖布局", NULL, ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize({ 600, 218 }, ImGuiCond_Once);
			ImGui::BeginChild("菜单", ImVec2(100, 0), true);
			if (test.ImageButton("主菜单", my_texture4, ImVec2(80, 50)))
			{
				menutap = 1;
			}
			if (test.ImageButton("绘制", my_texture4, ImVec2(80, 50)))
			{
				menutap = 2;
			}
			if (test.ImageButton("其他", my_texture4, ImVec2(80, 50)))
			{
				menutap = 3;
			}

			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("内容", ImVec2(0, 0), true);
			switch (menutap)
			{
			case 1:
				ImGui::Text("fps ： (%.3f FPS)", io.Framerate);
				if (ImGui::Button("退出"))
				{
					done = true;
				}
				{
					ImGuiKey start_key = (ImGuiKey)0;
					struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
					for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
					{
						if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;

						ImGui::Text("%s", ImGui::GetKeyName(key));
						ImGui::SameLine();

					}
				}
				break;
			case 2:
				test.CheckBox("绘制圆形", &show_Circle);
				test.CheckBox("测试绘图", &show_test);
				test.CheckBox("绘制射线", &show_line);
				test.CheckBox("绘制矩形", &show_box);
				break;
			case 3:
				test.ImageButton("仿王者button", my_texture4, ImVec2(445 / 2, 107 / 2));
				test.CheckBox("hello", &show_calculator);
				break;
			}

			ImGui::EndChild();


			ImGui::End();
		}

		{
			ImGuiWindowClass noAutoMerge{};
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
			ImGui::Begin("横布局");
			if (test.ImageButton("主菜单", my_texture4, ImVec2(80, 50)))
			{
				menutap = 1;
			}
			ImGui::SameLine();
			if (test.ImageButton("绘制", my_texture4, ImVec2(80, 50)))
			{
				menutap = 2;
			}
			ImGui::SameLine();
			if (test.ImageButton("其他", my_texture4, ImVec2(80, 50)))
			{
				menutap = 3;
			}
			switch (menutap)
			{
			case 1:
				ImGui::Text("fps ： (%.3f FPS)", io.Framerate);
				if (ImGui::Button("退出"))
				{
					done = true;
				}
				{
					ImGuiKey start_key = (ImGuiKey)0;
					struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
					for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
					{
						if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;

						ImGui::Text("%s", ImGui::GetKeyName(key));
						ImGui::SameLine();

					}
				}
				break;
			case 2:
				test.CheckBox("绘制圆形", &show_Circle);
				test.CheckBox("测试绘图", &show_test);
				test.CheckBox("绘制射线", &show_line);
				test.CheckBox("绘制矩形", &show_box);
				break;
			case 3:
				test.ImageButton("仿王者button", my_texture4, ImVec2(445 / 2, 107 / 2));
				test.CheckBox("hello", &show_calculator);
				break;
			}
			ImGui::End();
		}



		{
			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
			ImGui::Begin("背景图片");
			ImGui::SetWindowSize(ImVec2(jcly_image_width, jcly_image_height), ImGuiCond_Once);
			ImGuiWindow* TheWindow = ImGui::GetCurrentWindow();
			ImVec2 leftuppos = TheWindow->Pos;
			ImVec2 size = ImGui::GetWindowSize();
			ImVec2 rightdownpos;
			rightdownpos.x = leftuppos.x + size.x;
			rightdownpos.y = leftuppos.y + size.y;
			TheWindow->DrawList->AddImage(my_texture3, leftuppos, rightdownpos);
			ImGui::Button("1");
			ImGui::End();
		}
		
		
		{
			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);
			ImGui::Begin("by,,muchen");
			ImGui::SetWindowSize({ 200, 600 }, ImGuiCond_Once);
			if (ImGui::CollapsingHeader("功能"))
			{
				ImGui::Bullet();
				ImGui::Checkbox("简易计算器", &show_another_window);
				ImGui::Bullet();
				ImGui::Checkbox("简易仪表盘", &show_Speedometer);
				ImGui::Bullet();
				ImGui::Checkbox("计算器2.0", &show_calculator);
				ImGui::Bullet();
				ImGui::Checkbox("展示demo", &show_demo_window);
				ImGui::Bullet();
				ImGui::Checkbox("绘制圆形", &show_Circle);
				ImGui::Bullet();
				ImGui::Checkbox("测试绘图", &show_test);
				ImGui::Bullet();
				ImGui::Checkbox("绘制射线", &show_line);
				ImGui::Bullet();
				ImGui::Checkbox("绘制矩形", &show_box);
			}
			if (ImGui::CollapsingHeader("设置"))
			{
				if (ImGui::TreeNode("绘制项目"))
				{
					if (ImGui::TreeNode("圆角"))
					{
						ImGui::SliderFloat("矩形圆角", &box_yj, 1.0f, 100.0f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("速度"))
					{
						ImGui::SliderInt("速度", &speed, 1, 360);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("宽度"))
					{
						ImGui::SliderFloat("圆形粗细", &Circle_cx, 1.0f, 10.0f);
						ImGui::SliderFloat("射线粗细", &line_cx, 1.0f, 10.0f);
						ImGui::SliderFloat("矩形粗细", &box_cx, 1.0f, 10.0f);
						ImGui::SliderFloat("圆弧粗细", &arc_cx, 1.0f, 10.0f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("弧度"))
					{
						ImGui::SliderFloat("弧度", &hp, 1.0f, 100.0f);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode("颜色"))
					{
						ImGui::ColorEdit3("射线颜色", (float*)&line_color);
						ImGui::ColorEdit3("圆形颜色", (float*)&Circle_color);
						ImGui::ColorEdit3("矩形颜色", (float*)&box_color);

						ImGui::TreePop();
					}
					if (ImGui::TreeNode("位置"))
					{

						if (ImGui::TreeNode("射线位置"))
						{
							ImGui::SliderFloat("射线x1", &line_coord1.x, 1.0f, 1000.0f);
							ImGui::SliderFloat("射线y1", &line_coord1.y, 1.0f, 1000.0f);
							ImGui::SliderFloat("射线x2", &line_coord2.x, 1.0f, 1000.0f);
							ImGui::SliderFloat("射线y2", &line_coord2.y, 1.0f, 1000.0f);
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("矩形位置"))
						{
							ImGui::SliderFloat("矩形1x", &box_coord1.x, 1.0f, 1000.0f);
							ImGui::SliderFloat("矩形1y", &box_coord1.y, 1.0f, 1000.0f);
							ImGui::SliderFloat("矩形2x", &box_coord2.x, 1.0f, 1000.0f);
							ImGui::SliderFloat("矩形2y", &box_coord2.y, 1.0f, 1000.0f);
							ImGui::TreePop();
						}
						if (ImGui::TreeNode("圆形位置"))
						{
							ImGui::SliderFloat("圆形x1", &Circle_coord.x, 1.0f, 1000.0f);
							ImGui::SliderFloat("圆形y1", &Circle_coord.y, 1.0f, 1000.0f);
							ImGui::TreePop();
						}

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}



				if (ImGui::TreeNode("风格"))
				{
					if (ImGui::Combo("颜色", &style_idx, "白色\0蓝色\0紫色\0"))
					{
						switch (style_idx)
						{
						case 0:
							ImGui::StyleColorsLight();
							style.Colors[ImGuiCol_Header] = RGBAtoIV4(36, 54, 74, 79);
							break;
						case 1:
							ImGui::StyleColorsDark();
							break;
						case 2:
							ImGui::StyleColorsClassic();
							break;
						}
					}
					ImGui::ColorEdit3("字体颜色", (float*)&style.Colors[0]);

					ImGui::TreePop();
				}

			}
			ImGui::Text("fps ： (%.3f FPS)", io.Framerate);
			if (ImGui::Button("退出"))
			{
				done = true;
			}
			{
				ImGuiKey start_key = (ImGuiKey)0;
				struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
				for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
				{
					if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue;

					ImGui::Text("%s", ImGui::GetKeyName(key));
					ImGui::SameLine();

				}
			}
			ImGui::End();
		}
		if (show_another_window)
		{
			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);
			static int arr[2] = { 0,0 };
			static float arr1[2] = { 0,0 };
			ImGui::Begin("简易计算器");
			ImGui::SetWindowSize({ 600, 600 }, ImGuiCond_Once);

			if (ImGui::CollapsingHeader("加法"))
			{
				ImGui::SliderInt("被加数", &arr[0], 0, 10, "%d");
				ImGui::SliderInt("加数", &arr[1], 0, 10, "%d");
				ImGui::Text("结果=%d", arr[0] + arr[1]);
			}


			if (ImGui::CollapsingHeader("减法"))
			{
				ImGui::SliderInt("被减数", &arr[0], 0, 10, "%d");
				ImGui::SliderInt("减数", &arr[1], 0, 10, "%d");
				ImGui::Text("结果=%d", arr[0] - arr[1]);
			}


			if (ImGui::CollapsingHeader("乘法"))
			{
				ImGui::SliderInt("乘数1", &arr[0], 0, 10, "%d");
				ImGui::SliderInt("乘数2", &arr[1], 0, 10, "%d");
				ImGui::Text("结果=%d", arr[0] * arr[1]);
			}

			if (ImGui::CollapsingHeader("除法"))
			{
				ImGui::SliderFloat("被除数", &arr1[0], 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("除数", &arr1[1], 0.0f, 10.0f, "%.0f");
				ImGui::Text("结果=%.3f", arr1[0] / arr1[1]);
			}

			if (ImGui::Button("关闭这个窗口"))
			{
				show_another_window = false;
			}
			ImGui::End();
		}
		if (show_calculator)
		{

			ImGuiWindowClass noAutoMerge;
			noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
			ImGui::SetNextWindowClass(&noAutoMerge);
			ImGui::Begin("计算器 ");
			ImGui::Text(result.c_str());
			ImGui::Text("");
			if (ImGui::Button("C", ImVec2(50, 50)))
			{
				result.clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("+", ImVec2(50, 50)))
			{
				result += "+";
			}
			ImGui::SameLine();
			if (ImGui::Button("-", ImVec2(50, 50)))
			{
				result += "-";
			}
			ImGui::SameLine();
			if (ImGui::Button("<-", ImVec2(50, 50)))
			{
				result.pop_back();
			}
			if (ImGui::Button("7", ImVec2(50, 50)))
			{
				result += "7";
			}
			ImGui::SameLine();
			if (ImGui::Button("8", ImVec2(50, 50)))
			{
				result += "8";
			}
			ImGui::SameLine();
			if (ImGui::Button("9", ImVec2(50, 50)))
			{
				result += "9";
			}
			ImGui::SameLine();
			if (ImGui::Button("*", ImVec2(50, 50)))
			{
				result += "*";
			}
			if (ImGui::Button("4", ImVec2(50, 50)))
			{
				result += "4";
			}
			ImGui::SameLine();
			if (ImGui::Button("5", ImVec2(50, 50)))
			{
				result += "5";
			}
			ImGui::SameLine();
			if (ImGui::Button("6", ImVec2(50, 50)))
			{
				result += "6";
			}
			ImGui::SameLine();
			if (ImGui::Button("/", ImVec2(50, 50)))
			{
				result += "/";
			}
			if (ImGui::Button("1", ImVec2(50, 50)))
			{
				result += "1";
			}
			ImGui::SameLine();
			if (ImGui::Button("2", ImVec2(50, 50)))
			{
				result += "2";
			}
			ImGui::SameLine();
			if (ImGui::Button("3", ImVec2(50, 50)))
			{
				result += "3";
			}
			ImGui::SameLine();
			if (ImGui::Button(".", ImVec2(50, 50)))
			{
				result += ".";
			}
			if (ImGui::Button("(", ImVec2(50, 50)))
			{
				result += "(";
			}
			ImGui::SameLine();
			if (ImGui::Button("0", ImVec2(50, 50)))
			{
				result += "0";
			}
			ImGui::SameLine();
			if (ImGui::Button(")", ImVec2(50, 50)))
			{
				result += ")";
			}
			ImGui::SameLine();
			if (ImGui::Button("=", ImVec2(50, 50)))
			{
				double buff = evaluate(result);
				if (buff == static_cast<int>(buff)) // 检查结果是否为整数
					result = std::to_string(static_cast<int>(buff)); // 如果是整数，就转换为int类型
				else
					result = std::to_string(buff); // 如果不是整数，就保持为double类型
			}
			ImGui::End();
		}

		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_pSwapChain->Present(1, 0);
	}


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT* suggested_rect = (RECT*)lParam;
			::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}