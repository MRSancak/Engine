#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "../Editor/Editor.h"

namespace fs = std::filesystem;

static std::wstring GetText(HWND h)
{
    int len = GetWindowTextLengthW(h);
    std::wstring s(len, L'\0');
    GetWindowTextW(h, s.data(), len + 1);
    return s;
}

static bool ValidName(const std::wstring& name)
{
    if (name.empty()) return false;
    return name.find_first_of(L"<>:\"/\\|?*") == std::wstring::npos;
}

static fs::path ChooseFolder(HWND owner)
{
    BROWSEINFOW bi{};
    bi.hwndOwner = owner;
    bi.lpszTitle = L"Oyun projelerinin oluşturulacağı klasörü seç";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    PIDLIST_ABSOLUTE id = SHBrowseForFolderW(&bi);
    if (!id) return {};

    wchar_t buffer[MAX_PATH]{};
    SHGetPathFromIDListW(id, buffer);
    CoTaskMemFree(id);
    return fs::path(buffer);
}

static void CreateProject(const fs::path& p, const std::wstring& name)
{
    fs::create_directories(p / "Assets");
    fs::create_directories(p / "Scenes");
    fs::create_directories(p / "Scripts");
    fs::create_directories(p / "Settings");
    fs::create_directories(p / "Packages");

    std::ofstream config(p / "project.donder");
    config << "{\n"
              "  \"engine\": \"DonderEngine\",\n"
              "  \"version\": \"0.2.0\",\n"
              "  \"name\": \"" << std::string(name.begin(), name.end()) << "\",\n"
              "  \"startupScene\": \"Scenes/Main.scene\",\n"
              "  \"scriptLanguage\": \"C#\"\n"
              "}\n";

    std::ofstream scene(p / "Scenes" / "Main.scene");
    scene << "{\n"
             "  \"scene\": \"Main\",\n"
             "  \"objects\": [\n"
             "    {\"name\":\"Main Camera\"},\n"
             "    {\"name\":\"Directional Light\"},\n"
             "    {\"name\":\"Cube\"}\n"
             "  ]\n"
             "}\n";

    std::ofstream script(p / "Scripts" / "Game.cs");
    script << "using Donder;\n\n"
              "public class Game : MonoBehaviour\n"
              "{\n"
              "    public void Start() { }\n"
              "    public void Update() { }\n"
              "}\n";
}

class Launcher
{
public:
    HWND hwnd{};
    HWND name{};
    HWND path{};
    HWND list{};

    static LRESULT CALLBACK Proc(HWND h, UINT m, WPARAM w, LPARAM l)
    {
        Launcher* self = reinterpret_cast<Launcher*>(GetWindowLongPtrW(h, GWLP_USERDATA));

        if (m == WM_NCCREATE)
        {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(l);
            self = reinterpret_cast<Launcher*>(cs->lpCreateParams);
            self->hwnd = h;
            SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }

        if (self) return self->Handle(m, w, l);
        return DefWindowProcW(h, m, w, l);
    }

    LRESULT Handle(UINT m, WPARAM w, LPARAM l)
    {
        if (m == WM_CREATE)
        {
            CreateWindowW(L"STATIC", L"DONDER ENGINE",
                WS_CHILD | WS_VISIBLE, 30, 25, 500, 40, hwnd, nullptr, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"C++ 3D Game Engine  •  C# Scripting",
                WS_CHILD | WS_VISIBLE, 30, 65, 500, 25, hwnd, nullptr, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"PROJE OLUŞTUR",
                WS_CHILD | WS_VISIBLE,
                30, 105, 220, 42, hwnd, (HMENU)1, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Proje Adı",
                WS_CHILD | WS_VISIBLE, 30, 175, 100, 25, hwnd, nullptr, nullptr, nullptr);

            name = CreateWindowW(L"EDIT", L"YeniOyun",
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                30, 200, 350, 28, hwnd, nullptr, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"Konum",
                WS_CHILD | WS_VISIBLE, 30, 245, 100, 25, hwnd, nullptr, nullptr, nullptr);

            path = CreateWindowW(L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
                30, 270, 420, 28, hwnd, nullptr, nullptr, nullptr);

            CreateWindowW(L"BUTTON", L"Seç",
                WS_CHILD | WS_VISIBLE,
                460, 270, 80, 28, hwnd, (HMENU)2, nullptr, nullptr);

            CreateWindowW(L"STATIC", L"DİĞER PROJELER",
                WS_CHILD | WS_VISIBLE, 30, 325, 250, 30, hwnd, nullptr, nullptr, nullptr);

            list = CreateWindowW(L"LISTBOX", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY,
                30, 360, 510, 190, hwnd, (HMENU)3, nullptr, nullptr);

            Refresh();
            return 0;
        }

        if (m == WM_COMMAND)
        {
            int id = LOWORD(w);

            if (id == 2)
            {
                auto p = ChooseFolder(hwnd);
                if (!p.empty()) SetWindowTextW(path, p.c_str());
            }

            if (id == 1)
            {
                std::wstring projectName = GetText(name);
                std::wstring parent = GetText(path);

                if (!ValidName(projectName))
                {
                    MessageBoxW(hwnd, L"Geçerli bir proje adı gir.", L"Hata", MB_OK | MB_ICONERROR);
                    return 0;
                }

                if (parent.empty())
                {
                    MessageBoxW(hwnd, L"Proje konumunu seç.", L"Hata", MB_OK | MB_ICONERROR);
                    return 0;
                }

                fs::path project = fs::path(parent) / projectName;

                if (fs::exists(project))
                {
                    MessageBoxW(hwnd, L"Bu isimde bir proje zaten var.", L"Hata", MB_OK | MB_ICONERROR);
                    return 0;
                }

                CreateProject(project, projectName);
                Refresh();
                Donder::Editor::Show(project.wstring());
            }

            if (id == 3 && HIWORD(w) == LBN_DBLCLK)
            {
                int index = (int)SendMessageW(list, LB_GETCURSEL, 0, 0);
                if (index >= 0)
                {
                    wchar_t text[512]{};
                    SendMessageW(list, LB_GETTEXT, index, (LPARAM)text);
                    Donder::Editor::Show(
                        (fs::current_path() / "Projects" / text).wstring());
                }
            }
        }

        if (m == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(hwnd, m, w, l);
    }

    void Refresh()
    {
        SendMessageW(list, LB_RESETCONTENT, 0, 0);

        fs::path root = fs::current_path() / "Projects";
        fs::create_directories(root);

        for (auto& e : fs::directory_iterator(root))
        {
            if (e.is_directory() && fs::exists(e.path() / "project.donder"))
            {
                auto n = e.path().filename().wstring();
                SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)n.c_str());
            }
        }
    }
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSW wc{};
    wc.lpfnWndProc = Launcher::Proc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"DonderLauncherWindow";
    RegisterClassW(&wc);

    Launcher app;

    HWND hwnd = CreateWindowExW(
        0,
        L"DonderLauncherWindow",
        L"Donder Engine - Project Manager",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        610, 620,
        nullptr, nullptr, hInstance, &app);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CoUninitialize();
    return 0;
}
