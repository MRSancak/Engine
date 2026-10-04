#include "GameObject.h"

namespace Donder
{
    GameObject::GameObject(const std::string& name) : name_(name) {}

    const std::string& GameObject::GetName() const
    {
        return name_;
    }

    void GameObject::SetName(const std::string& name)
    {
        name_ = name;
    }

    Transform& GameObject::GetTransform()
    {
        return transform_;
    }

    const Transform& GameObject::GetTransform() const
    {
        return transform_;
    }
}
