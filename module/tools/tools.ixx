module;

#include "public.h"

export module tools;

export ID3D11Device* g_pd3dDevice;


export ImVec4 RGBAtoIV4(int r, int g, int b, int a);
export double evaluate(std::string tokens);
export bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
export std::string WideCharToMultiByte(const std::wstring& wstr);