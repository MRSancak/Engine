#pragma once

namespace Donder
{
    struct Vector3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vector3() = default;
        Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    };

    struct Transform
    {
        Vector3 position{0, 0, 0};
        Vector3 rotation{0, 0, 0};
        Vector3 scale{1, 1, 1};
    };
}
