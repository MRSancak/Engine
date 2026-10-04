#pragma once
#include <string>
#include <vector>
#include "GameObject.h"

namespace Donder
{
    class Engine
    {
    public:
        bool Start(const std::wstring& projectPath);
        void Run();
        void Shutdown();

        std::vector<GameObject>& GetScene();

    private:
        std::wstring projectPath_;
        bool running_ = false;
        std::vector<GameObject> scene_;
    };
}
