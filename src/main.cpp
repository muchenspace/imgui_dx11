#include "main.h"

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
    D3D11_SUBRESOURCE_DATA subResource;
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





     HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"imgui",WS_POPUP, 0, 0, 2560, 1440, nullptr, nullptr, wc.hInstance, nullptr);//隐藏标题
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


    ImFont* Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    ImFont* Font_Big = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 24.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());



    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr,io.Fonts->GetGlyphRangesChineseFull());
    style.FrameRounding = 12;
    ImGui::StyleColorsLight();
    style.Colors[ImGuiCol_Header] = RGBAtoIV4(36, 54, 74, 79);

    int my_image_width;//图长
    int my_image_height;//图宽
    ID3D11ShaderResourceView* my_texture = NULL;
    bool ret = LoadTextureFromFile("jcly.png", &my_texture,&my_image_width, &my_image_height);
    IM_ASSERT(ret);



   
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
     
        
        static bool show_demo_window{};
        static bool show_another_window{};
        static bool show_line{};
        static bool show_Circle{};
        static bool show_box{};

       
        static float line_cx = 2;
        static float Circle_cx = 2;
        static float box_cx = 2;


        static float box_yj = 20;

        static ImVec2 line_coord1{ 200,200 };
        static ImVec2 line_coord2{ 200,400 };
        static ImVec2 box_coord1{ 200,300 };
        static ImVec2 box_coord2{ 350,550 };
        static ImVec2 Circle_coord{ 400,800 };


        static ImColor Circle_color = ImColor{ 255, 0, 0 };
        static ImVec4  clear_color = ImVec4{ 0, 0, 0, 0 };
        static ImColor line_color = ImColor{ 255, 0, 0 };
        static ImColor box_color = ImColor{ 43, 255, 0 };
        static ImGuiStyle ref_saved_style;
        static int style_idx = 0;
        //////////////////////////////////定义区结束
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置优先级别最高
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
        {
            ImGuiWindowClass noAutoMerge;
            noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
            ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
            ImGui::Begin("123");
            ImGuiWindow* TheWindow = ImGui::GetCurrentWindow();
            ImVec2 leftuppos = TheWindow->Pos;
            ImVec2 size = ImGui::GetWindowSize();
            ImVec2 rightdownpos;
            rightdownpos.x = leftuppos.x + size.x ;
            rightdownpos.y = leftuppos.y + size.y;
            TheWindow->DrawList->AddImage(my_texture,leftuppos,rightdownpos);
            ImGui::Button("test");
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
                ImGui::Checkbox("简易计算器", &show_another_window);
                ImGui::Checkbox("展示demo", &show_demo_window);
                ImGui::Checkbox("绘制圆形", &show_Circle);
                ImGui::Checkbox("绘制射线", &show_line);
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
                    if (ImGui::TreeNode("宽度"))
                    {
                        ImGui::SliderFloat("圆形粗细", &Circle_cx, 1.0f, 10.0f);
                        ImGui::SliderFloat("射线粗细", &line_cx, 1.0f, 10.0f);
                        ImGui::SliderFloat("矩形粗细", &box_cx, 1.0f, 10.0f);
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
                exit(0);
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
