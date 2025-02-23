#include "camera.h"

#include <windows.h>
#include <glm/gtx/transform.hpp>
#include <minhook.h>

#include <imgui.h>

char** camera = reinterpret_cast<char**>(0x009de078);
glm::mat4* matrix = reinterpret_cast<glm::mat4*>(*camera);

typedef void(__cdecl* MTH_Matrix44Copy)(glm::mat4* copyTo, glm::mat4* copyFrom);
MTH_Matrix44Copy pMTH_Matrix44CopyTarget = reinterpret_cast<MTH_Matrix44Copy>(0x006c1170);
MTH_Matrix44Copy pMTH_Matrix44Copy = nullptr;

void __cdecl detour_MTH_Matrix44Copy(glm::mat4* copyTo, glm::mat4* copyFrom)
{
    if (Camera::Mode != CAMMODE_REGULAR && copyTo == matrix)
        return;

    pMTH_Matrix44Copy(copyTo, copyFrom);
}

typedef void(__fastcall* SetFieldOfVision)(void* self, float fov);
SetFieldOfVision pSetFieldOfVisionTarget = reinterpret_cast<SetFieldOfVision>(0x0040d860);
SetFieldOfVision pSetFieldOfVision = nullptr;

void __fastcall detour_SetFieldOfVision(void* self, float fov)
{
    if (self == *camera)
    {
        pSetFieldOfVision(self, Camera::Fov);
        return;
    }

    pSetFieldOfVision(self, fov);
}

typedef void(__fastcall* SetNearAndFarPlane)(void* self, float nearPlane, float farPlane);
SetNearAndFarPlane pSetNearAndFarPlaneTarget = reinterpret_cast<SetNearAndFarPlane>(0x00437520);
SetNearAndFarPlane pSetNearAndFarPlane = nullptr;

void __fastcall detour_SetNearAndFarPlane(void* self, float nearPlane, float farPlane)
{
    if (self == *camera)
    {
        pSetNearAndFarPlane(self, Camera::NearPlane, Camera::FarPlane);
        return;
    }

    pSetNearAndFarPlane(self, nearPlane, farPlane);
}

int   Camera::Mode      = CAMMODE_REGULAR;
float Camera::Fov       = glm::radians(60.0f);
float Camera::NearPlane = 0.0f;
float Camera::FarPlane  = 1000.0f;

void Camera::CreateHooks()
{
    if (MH_CreateHook(reinterpret_cast<LPVOID*>(pMTH_Matrix44CopyTarget), &detour_MTH_Matrix44Copy, reinterpret_cast<LPVOID*>(&pMTH_Matrix44Copy)) != MH_OK)
        printf("%s hook failed\n", "MTH_Matrix44Copy");

    if (MH_CreateHook(reinterpret_cast<LPVOID*>(pSetFieldOfVisionTarget), &detour_SetFieldOfVision, reinterpret_cast<LPVOID*>(&pSetFieldOfVision)) != MH_OK)
        printf("%s hook failed\n", "SetFieldOfVision");

    if (MH_CreateHook(reinterpret_cast<LPVOID*>(pSetNearAndFarPlaneTarget), &detour_SetNearAndFarPlane, reinterpret_cast<LPVOID*>(&pSetNearAndFarPlane)) != MH_OK)
        printf("%s hook failed\n", "SetNearAndFarPlane");
}

void Camera::Draw()
{
    ImGui::SeparatorText("Camera");
    ImGui::Text("Mode");
    ImGui::RadioButton("Regular",  &Camera::Mode, CAMMODE_REGULAR ); ImGui::SameLine();
    ImGui::RadioButton("Detached", &Camera::Mode, CAMMODE_DETACHED); ImGui::SameLine();
    ImGui::RadioButton("Freecam",  &Camera::Mode, CAMMODE_FREECAM );

    ImGui::Text("Settings");
    ImGui::SliderAngle("FOV", &Camera::Fov, 0.0f, 180.0f, "%.2f");
    ImGui::SliderFloat("Near Plane", &Camera::NearPlane, 0.0f, 10000.0f, "%.2f");
    ImGui::SliderFloat("Far Plane", &Camera::FarPlane, 0.0f, 10000.0f, "%.2f");
}

void Camera::Update()
{
    if (Mode != CAMMODE_FREECAM)
        return;

    if (GetKeyState('W') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, 0.0f, 1.0f));

    if (GetKeyState('S') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, 0.0f, -1.0f));

    if (GetKeyState('A') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(-1.0f, 0.0f, 0.0f));

    if (GetKeyState('D') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(1.0f, 0.0f, 0.0f));

    if (GetKeyState('E') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, -1.0f, 0.0f));

    if (GetKeyState('Q') & 0x8000)
        *matrix = glm::translate(*matrix, glm::vec3(0.0f, 1.0f, 0.0f));

    if (GetKeyState(VK_LEFT) & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(-1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (GetKeyState(VK_RIGHT) & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (GetKeyState(VK_UP) & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    if (GetKeyState(VK_DOWN) & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(-1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    if (GetKeyState('R') & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(-1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    if (GetKeyState('T') & 0x8000)
        *matrix = glm::rotate(*matrix, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}