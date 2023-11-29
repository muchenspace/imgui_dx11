#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "Font.h"
#include <d3d11.h>
#include <stdio.h>
#include "imgui_internal.h"

#pragma comment(lib,"d3d11.lib")
#pragma execution_character_set("utf-8")
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


ImVec4 RGBAtoIV4(int r, int g, int b, int a) {
    float newr = r / 255.0f;
    float newg = g / 255.0f;
    float newb = b / 255.0f;
    float newa = a / 255.0f;
    return ImVec4(newr, newg, newb, newa);
}
