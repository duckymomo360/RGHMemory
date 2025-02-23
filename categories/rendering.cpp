#include "rendering.h"
#include <imgui.h>

typedef unsigned long ulong;

ulong* K3D_C_MaxVisibleSector = reinterpret_cast<ulong*>(0x0099156c);
ulong* K3D_C_MaxSubVisibleSector = reinterpret_cast<ulong*>(0x00991570);

void Rendering::Draw()
{
    ImGui::SeparatorText("Rendering");
    ImGui::SliderInt("K3D_C_MaxVisibleSector", (int*)K3D_C_MaxVisibleSector, 0, 50);
    ImGui::SliderInt("K3D_C_MaxSubVisibleSector", (int*)K3D_C_MaxSubVisibleSector, 0, 500);
}