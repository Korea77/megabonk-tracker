#include "assert/MinHook.h"
#include "HookDX11.h"
#include <dxgi.h>

typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present oPresent = nullptr;




typedef LRESULT(CALLBACK* WNDPROC_TYPE)(HWND, UINT, WPARAM, LPARAM);


LRESULT CALLBACK hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return PanelUI::WndProcHandler(hWnd, msg, wParam, lParam);
}
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool init = false;
    static HWND hwnd = nullptr;

    if (!init) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&PanelUI::pDevice))) {
            PanelUI::pDevice->GetImmediateContext(&PanelUI::pContext);

            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
            PanelUI::pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &PanelUI::pRenderTargetView);
            pBackBuffer->Release();


            hwnd = FindWindowA(NULL, "Megabonk");
            PanelUI::InitImGui(hwnd);

            // Hook WndProc
            PanelUI::oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);


            init = true;


        }
    }

    PanelUI::Render();
    return oPresent(pSwapChain, SyncInterval, Flags);
}

bool HookDX11(HWND hwnd) {

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* pDeviceTemp = nullptr;
    ID3D11DeviceContext* pContextTemp = nullptr;
    IDXGISwapChain* pSwapChainTemp = nullptr;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
        &scd, &pSwapChainTemp, &pDeviceTemp, nullptr, &pContextTemp))) {
        return false;
    }

    void** pVTable = *reinterpret_cast<void***>(pSwapChainTemp);
    MH_CreateHook(pVTable[8], &hkPresent, reinterpret_cast<void**>(&oPresent));
    MH_EnableHook(MH_ALL_HOOKS);

    pSwapChainTemp->Release();
    pDeviceTemp->Release();
    pContextTemp->Release();

    return true;
}
