#pragma once
#include <string>
#include <windows.h>
#include "../Engine/Engine.h"

namespace Donder
{
    class Editor
    {
    public:
        static int Show(const std::wstring& projectPath);

    private:
        static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
        static void DrawViewport(HDC, RECT, float, float);
        static void DrawGrid(HDC, RECT);
        static void DrawCube(HDC, RECT);

        static Engine* engine_;
        static float cameraX_;
        static float cameraY_;
        static float cameraZoom_;
    };
}
