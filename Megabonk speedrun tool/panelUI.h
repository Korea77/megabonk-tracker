#pragma once
#include <d3d11.h>
#include <Windows.h>


typedef class _Game;
extern _Game* Game;

namespace PanelUI {

    extern ID3D11Device* pDevice;
    extern ID3D11DeviceContext* pContext;
    extern ID3D11RenderTargetView* pRenderTargetView;

    void InitImGui(HWND hwnd);
    void Render();
    void Cleanup();

    // WndProc handler dla ImGui
    LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    
    extern bool showMenu;
    extern WNDPROC oWndProc;

}