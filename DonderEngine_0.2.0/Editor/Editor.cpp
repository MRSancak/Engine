#include "Editor.h"
#include <windows.h>
#include <string>
#include <sstream>
#include <cmath>

namespace Donder
{
    Engine* Editor::engine_ = nullptr;
    float Editor::cameraX_ = 0.0f;
    float Editor::cameraY_ = 0.0f;
    float Editor::cameraZoom_ = 1.0f;

    static void Text(HDC dc, int x, int y, const std::wstring& s)
    {
        TextOutW(dc, x, y, s.c_str(), static_cast<int>(s.size()));
    }

    static void Panel(HDC dc, RECT r)
    {
        FillRect(dc, &r, reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1));
        FrameRect(dc, &r, reinterpret_cast<HBRUSH>(COLOR_BTNSHADOW + 1));
    }

    int Editor::Show(const std::wstring& projectPath)
    {
        Engine engine;
        engine_ = &engine;
        engine.Start(projectPath);

        HINSTANCE instance = GetModuleHandleW(nullptr);
        const wchar_t* cls = L"DonderEditorWindow";

        WNDCLASSW wc{};
        wc.lpfnWndProc = Editor::WndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = cls;
        RegisterClassW(&wc);

        HWND hwnd = CreateWindowExW(
            0, cls, L"Donder Engine - Editor",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1280, 760,
            nullptr, nullptr, instance, nullptr);

        if (!hwnd)
            return 1;

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        engine.Shutdown();
        engine_ = nullptr;
        return 0;
    }

    LRESULT CALLBACK Editor::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        switch (msg)
        {
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE)
                DestroyWindow(hwnd);

            if (wp == VK_ADD || wp == VK_OEM_PLUS)
                cameraZoom_ += 0.1f;

            if (wp == VK_SUBTRACT || wp == VK_OEM_MINUS)
                cameraZoom_ = (cameraZoom_ > 0.2f) ? cameraZoom_ - 0.1f : cameraZoom_;

            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_MOUSEWHEEL:
        {
            short delta = GET_WHEEL_DELTA_WPARAM(wp);
            cameraZoom_ += delta > 0 ? 0.1f : -0.1f;
            if (cameraZoom_ < 0.2f) cameraZoom_ = 0.2f;
            if (cameraZoom_ > 4.0f) cameraZoom_ = 4.0f;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);

            RECT client{};
            GetClientRect(hwnd, &client);

            FillRect(dc, &client, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));

            RECT toolbar{0, 0, client.right, 42};
            FillRect(dc, &toolbar, reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1));
            FrameRect(dc, &toolbar, reinterpret_cast<HBRUSH>(COLOR_BTNSHADOW + 1));

            Text(dc, 15, 12, L"File");
            Text(dc, 55, 12, L"Edit");
            Text(dc, 95, 12, L"GameObject");
            Text(dc, 185, 12, L"Component");
            Text(dc, 285, 12, L"Window");
            Text(dc, 350, 12, L"Help");

            RECT play{470, 7, 530, 35};
            FrameRect(dc, &play, reinterpret_cast<HBRUSH>(COLOR_BTNSHADOW + 1));
            Text(dc, 482, 12, L"▶");

            RECT stop{535, 7, 595, 35};
            FrameRect(dc, &stop, reinterpret_cast<HBRUSH>(COLOR_BTNSHADOW + 1));
            Text(dc, 548, 12, L"■");

            int hierarchyWidth = 230;
            int inspectorWidth = 270;
            int bottomHeight = 150;

            RECT hierarchy{
                0, 42,
                hierarchyWidth,
                client.bottom - bottomHeight
            };

            RECT viewport{
                hierarchyWidth, 42,
                client.right - inspectorWidth,
                client.bottom - bottomHeight
            };

            RECT inspector{
                client.right - inspectorWidth, 42,
                client.right,
                client.bottom - bottomHeight
            };

            RECT assets{
                0,
                client.bottom - bottomHeight,
                client.right / 2,
                client.bottom
            };

            RECT console{
                client.right / 2,
                client.bottom - bottomHeight,
                client.right,
                client.bottom
            };

            Panel(dc, hierarchy);
            Panel(dc, inspector);
            Panel(dc, assets);
            Panel(dc, console);

            DrawViewport(dc, viewport.left, viewport.top);
            DrawGrid(dc, viewport);
            DrawCube(dc, viewport);

            Text(dc, 15, 58, L"HIERARCHY");

            int y = 90;
            for (const auto& object : engine_->GetScene())
            {
                std::wstring line = L"  " + std::wstring(object.GetName().begin(), object.GetName().end());
                Text(dc, 15, y, line);
                y += 28;
            }

            Text(dc, inspector.left + 15, 58, L"INSPECTOR");
            Text(dc, inspector.left + 15, 92, L"Transform");
            Text(dc, inspector.left + 30, 125, L"Position");
            Text(dc, inspector.left + 45, 150, L"X: 0.00");
            Text(dc, inspector.left + 45, 172, L"Y: 0.00");
            Text(dc, inspector.left + 45, 194, L"Z: 0.00");
            Text(dc, inspector.left + 30, 225, L"Rotation");
            Text(dc, inspector.left + 30, 265, L"Scale");
            Text(dc, inspector.left + 45, 290, L"X: 1.00   Y: 1.00   Z: 1.00");

            Text(dc, 15, assets.top + 22, L"ASSETS");
            Text(dc, 30, assets.top + 52, L"Assets/");
            Text(dc, 45, assets.top + 78, L"  Scenes/");
            Text(dc, 45, assets.top + 104, L"  Scripts/");
            Text(dc, 30, assets.top + 130, L"  Main.scene");

            Text(dc, console.left + 15, console.top + 22, L"CONSOLE");
            Text(dc, console.left + 30, console.top + 52, L"[Donder] Editor started.");
            Text(dc, console.left + 30, console.top + 78, L"[Donder] Scene loaded.");
            Text(dc, console.left + 30, console.top + 104, L"[Donder] 3 GameObjects.");

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    void Editor::DrawViewport(HDC dc, RECT viewport)
    {
        HBRUSH brush = CreateSolidBrush(RGB(45, 45, 48));
        FillRect(dc, &viewport, brush);
        DeleteObject(brush);

        Text(dc, viewport.left + 15, viewport.top + 15, L"SCENE");
        Text(dc, viewport.right - 90, viewport.top + 15, L"Perspective");
    }

    void Editor::DrawGrid(HDC dc, RECT r)
    {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(65, 65, 68));
        HPEN old = reinterpret_cast<HPEN>(SelectObject(dc, pen));

        const int spacing = 32;
        int cx = (r.left + r.right) / 2 + static_cast<int>(cameraX_);
        int cy = (r.top + r.bottom) / 2 + static_cast<int>(cameraY_);

        for (int x = cx % spacing; x < r.right; x += spacing)
            MoveToEx(dc, x, r.top, nullptr), LineTo(dc, x, r.bottom);

        for (int y = cy % spacing; y < r.bottom; y += spacing)
            MoveToEx(dc, r.left, y, nullptr), LineTo(dc, r.right, y);

        SelectObject(dc, old);
        DeleteObject(pen);
    }

    void Editor::DrawCube(HDC dc, RECT r)
    {
        int cx = (r.left + r.right) / 2 + static_cast<int>(cameraX_);
        int cy = (r.top + r.bottom) / 2 + static_cast<int>(cameraY_);

        int size = static_cast<int>(70 * cameraZoom_);
        POINT front[4]{
            {cx - size, cy - size},
            {cx + size, cy - size},
            {cx + size, cy + size},
            {cx - size, cy + size}
        };

        POINT back[4]{
            {cx - size / 2, cy - size * 3 / 2},
            {cx + size * 3 / 2, cy - size * 3 / 2},
            {cx + size * 3 / 2, cy + size / 2},
            {cx - size / 2, cy + size / 2}
        };

        HPEN pen = CreatePen(PS_SOLID, 2, RGB(220, 220, 220));
        HPEN old = reinterpret_cast<HPEN>(SelectObject(dc, pen));

        Polygon(dc, front, 4);
        Polygon(dc, back, 4);

        for (int i = 0; i < 4; ++i)
        {
            MoveToEx(dc, front[i].x, front[i].y, nullptr);
            LineTo(dc, back[i].x, back[i].y);
        }

        SelectObject(dc, old);
        DeleteObject(pen);

        Text(dc, cx - 20, cy + size + 20, L"Cube");
    }
}
