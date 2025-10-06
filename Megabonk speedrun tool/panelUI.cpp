#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <D3DX11.h>
#include <dwmapi.h>

#include<string>
#include "assert/robotoFont.h"
#include "utils.h"
#include "Game.h"
#include "PanelUI.h"
#include <map>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



namespace PanelUI {
    bool showMenu = true;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    ID3D11RenderTargetView* pRenderTargetView = nullptr;
    WNDPROC oWndProc = nullptr;
    
    void InitImGui(HWND hwnd) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(pDevice, pContext);
    }


    ImVec4 GetItemColor(EItemRarity rarity) {
        switch (rarity) {
        case EItemRarity::Common:     return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // bia³y
        case EItemRarity::Rare:       return ImVec4(0.0f, 0.5f, 1.0f, 1.0f);   // niebieski
        case EItemRarity::Epic:       return ImVec4(0.7f, 0.0f, 0.7f, 1.0f);   // fioletowy
        case EItemRarity::Legendary:  return ImVec4(1.0f, 0.8f, 0.0f, 1.0f);   // pomarañczowy
        case EItemRarity::Corrupted:  return ImVec4(0.5f, 0.0f, 0.0f, 1.0f);   // ciemnoczerwony
        case EItemRarity::Quest:      return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);   // ¿ó³ty
        default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // fallback: bia³y
        }
    }

    void RenderMerchantMap(const std::map<EItemRarity, int>& merchantMap) {
        for (auto& [rarity, count] : merchantMap) {
            ImGui::SameLine();
            ImVec4 color = GetItemColor(rarity);
            ImGui::TextColored(color, "%d", count);
            
        }

    }

    bool isFontSetup = false;
    void initalize_style() { // used from my other project
        

        if (!isFontSetup) {
            ImGuiIO& io = ImGui::GetIO();

            ImFontConfig font_cfg;
            font_cfg.FontDataOwnedByAtlas = false;
            font_cfg.OversampleH = 3;
            font_cfg.OversampleV = 1;
            font_cfg.PixelSnapH = false;


            ImFont* roboto_font = io.Fonts->AddFontFromMemoryTTF(
                (void*)robotoFont,
                168260,
                14,
                &font_cfg
            );



            io.FontDefault = roboto_font;



            ImGui_ImplDX11_InvalidateDeviceObjects();
            ImGui_ImplDX11_CreateDeviceObjects();
            isFontSetup = true;
        }
        static bool imgui_initStyle = false;
        if (imgui_initStyle) {
            return;
        }
        imgui_initStyle = true;
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        style->WindowBorderSize = 4.0f;
        style->WindowRounding = 5.0f;          
        style->FrameRounding = 4.0f;           
        style->GrabRounding = 4.0f;           
        style->TabRounding = 4.0f;            

        style->WindowBorderSize = 1.0f;
        style->FrameRounding = 6.0f;
        style->GrabRounding = 4.0f;

        style->FramePadding = ImVec2(10, 6);
        style->ItemSpacing = ImVec2(8, 6);
        
        ImVec4 bg = ImVec4(0.1, 0.1, 0.1, 1); 
        ImVec4 hover = ImVec4(0.12, 0.12, 0.12, 1); 
        ImVec4 active = ImVec4(0.12, 0.12, 0.12, 1); 



        style->Colors[ImGuiCol_Border] = ImVec4(0.92f, 0.25f, 0.20f, 0.5f);
        style->Colors[ImGuiCol_FrameBg] = bg;
        style->Colors[ImGuiCol_FrameBgHovered] = hover;
        style->Colors[ImGuiCol_FrameBgActive] = active;
        style->Colors[ImGuiCol_Button] = bg;
        style->Colors[ImGuiCol_ButtonHovered] = hover;
        style->Colors[ImGuiCol_ButtonActive] = active;
        style->Colors[ImGuiCol_Header] = bg;
        style->Colors[ImGuiCol_HeaderHovered] = hover;
        style->Colors[ImGuiCol_HeaderActive] = active;
        style->Colors[ImGuiCol_PopupBg] = bg;
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.92f, 0.25f, 0.20f, 1);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.92f, 0.25f, 0.20f, 1);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.25f, 0.20f, 1);

    }

    void Render() {
        if (!showMenu) return;

        if (!pContext || !pRenderTargetView) return;

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        initalize_style();
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        ImGui::SetNextWindowSize(ImVec2(screenWidth * 0.15, -1), 0);
        ImVec2 windowSize = ImGui::GetIO().DisplaySize;
        ImGui::Begin(" ", nullptr, window_flags);

        std::string chestSTR = "Free chests: ";
        

        chestSTR += std::to_string(Game->Chests.getFreeChests());
        ImGui::Text("Merchants:");
        RenderMerchantMap(Game->Merchants.getMerchants());

        ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), chestSTR.c_str());
        std::string expGround = "EXP on ground: " + std::to_string(Game->Pickups.getExpOnGround());
        ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), expGround.c_str());
        
        
        ImGui::Text("Microwaves:");
        RenderMerchantMap(Game->Microwaves.getMicrowaves());

        std::string moaiShrines = "Moai shrines: " + std::to_string(Game->MoaiShrines.count());
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), moaiShrines.c_str());

        std::string magnetShrines = "Magnet shrines: " + std::to_string(Game->MagnetShrines.count());
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), magnetShrines.c_str());

        magnetShrines = "Cursed shrines: " + std::to_string(Game->CursedShrines.count());
        ImGui::TextColored(ImVec4(0.92f, 0.25f, 0.20f, 1.0f), magnetShrines.c_str());

        magnetShrines = "Challenge shrines: " + std::to_string(Game->ChallengeShrines.count());
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), magnetShrines.c_str());

        magnetShrines = "Totems: " + std::to_string(Game->GreedShrines.count());
        ImGui::TextColored(ImVec4(0.92f, 0.25f, 0.20f, 1.0f), magnetShrines.c_str());



        
        //const char* text = "by Korea";
        //ImVec2 textSize = ImGui::CalcTextSize(text);

        //// ustaw pozycjê kursora tak, by tekst by³ w prawym dolnym rogu
        //ImVec2 s = ImGui::GetWindowSize();
        //ImGui::SetCursorPosY(s.y - textSize.y - ImGui::GetTextLineHeight());
        //ImGui::SetCursorPosX(s.x - textSize.x * 1.5);
        //ImGui::Text(text);

        ImGui::NewLine();
        ImGui::End();
        
        ImGui::Render();
        pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    void Cleanup() {
        if (pRenderTargetView) { pRenderTargetView->Release(); pRenderTargetView = nullptr; }
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    
    LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_KEYUP && wParam == VK_INSERT) {
            showMenu = !showMenu;
            return 0;
        }

        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        return oWndProc(hWnd, msg, wParam, lParam);
    }

    
}
