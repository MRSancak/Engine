#include "Engine.h"

namespace Donder
{
    bool Engine::Start(const std::wstring& projectPath)
    {
        projectPath_ = projectPath;
        running_ = true;

        scene_.clear();
        scene_.emplace_back("Main Camera");
        scene_.emplace_back("Directional Light");
        scene_.emplace_back("Cube");

        return true;
    }

    void Engine::Run()
    {
        // Editor owns the Windows message loop in this version.
    }

    void Engine::Shutdown()
    {
        running_ = false;
    }

    std::vector<GameObject>& Engine::GetScene()
    {
        return scene_;
    }
}
