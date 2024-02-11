#include <windows.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <minhook.h>

#include "imgui/imgui.h"
#include "ui.h"

#include "categories/camera.h"
#include "categories/rendering.h"

void UI::Draw()
{
    ImGui::Begin("LyN Tools", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

    Camera::Draw();
    Rendering::Draw();

    ImGui::End();
}

bool CreateHooks()
{
    if (MH_Initialize() != MH_OK) {
        printf("Failed to initialize MinHook\n");
        return FALSE;
    }

    Camera::CreateHooks();

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        printf("Enabling hooks failed\n");
        return FALSE;
    }

    return TRUE;
}

DWORD WINAPI Main(LPVOID lpThreadParameter)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    if (!CreateHooks())
        return FALSE;

    if (!UI::Init())
        return FALSE;

    printf("Started\n");

    while (true)
    {
        Camera::Update();
        Sleep(10);
    }

    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, Main, hModule, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

