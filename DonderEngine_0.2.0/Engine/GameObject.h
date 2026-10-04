#pragma once
#include <string>
#include "Math.h"

namespace Donder
{
    class GameObject
    {
    public:
        explicit GameObject(const std::string& name);

        const std::string& GetName() const;
        void SetName(const std::string& name);

        Transform& GetTransform();
        const Transform& GetTransform() const;

    private:
        std::string name_;
        Transform transform_;
    };
}
