
#include <windows.h>
#include <d3d9.h>

static HMODULE g_systemD3D9 = nullptr;

using D3D9CreateFn = IDirect3D9* (WINAPI*)(UINT);

static D3D9CreateFn g_create9 = nullptr;

static bool InitializeD3D9()
{
    if (g_create9)
        return true;

    wchar_t path[MAX_PATH];

    UINT len = GetSystemDirectoryW(path, MAX_PATH);

    if (!len || len >= MAX_PATH - 10)
        return false;

    lstrcatW(path, L"\\d3d9.dll");

    g_systemD3D9 = LoadLibraryW(path);

    if (!g_systemD3D9)
        return false;

    g_create9 = reinterpret_cast<D3D9CreateFn>(
        GetProcAddress(g_systemD3D9, "Direct3DCreate9")
    );

    return g_create9 != nullptr;
}

extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT sdkVersion)
{
    if (!InitializeD3D9())
        return nullptr;

    return g_create9(sdkVersion);
}

#ifdef _M_IX86
#pragma comment(linker, "/EXPORT:Direct3DCreate9=_Direct3DCreate9@4")
#endif

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls(instance);

    return TRUE;
}
