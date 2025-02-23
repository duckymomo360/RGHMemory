#include "ui.h"

#include <stdio.h>
#include <d3d9.h>
#include "kiero.h"
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
static EndScene oEndScene = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC oWndProc = NULL;

LRESULT CALLBACK hkWindowProc(
    _In_ HWND   hwnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0)
        return 1L;

    return CallWindowProcA(oWndProc, hwnd, uMsg, wParam, lParam);
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
    static bool init = false;

    if (!init)
    {
        D3DDEVICE_CREATION_PARAMETERS params;
        pDevice->GetCreationParameters(&params);

        oWndProc = (WNDPROC)SetWindowLongPtr(params.hFocusWindow, GWLP_WNDPROC, (LONG)hkWindowProc);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(params.hFocusWindow);
        ImGui_ImplDX9_Init(pDevice);

        init = true;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI::Draw();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    return oEndScene(pDevice);
}

bool UI::Init()
{
    if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
    {
        if (kiero::bind(42, (void**)&oEndScene, hkEndScene) == kiero::Status::Success) {
            return TRUE;
        }
    }

    printf("Kiero init failed\n");

    return FALSE;
}