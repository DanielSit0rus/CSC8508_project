#pragma once
#include "Vector.h"

using namespace NCL::Maths;

class Light
{
    public:
    
        Light(const Vector3& pos, const Vector4& col, float rad)
            : position(pos), color(col), radius(rad) {}

        Vector3 GetPosition() const { return position; }
        Vector4 GetColor() const { return color; }
        float GetRadius() const { return radius; }

        void SetPosition(const Vector3& pos) { position = pos; }
        void SetColor(const Vector4& col) { color = col; }
        void SetRadius(float rad) { radius = rad; }

    private:
        Vector3 position;
        Vector4 color;
        float radius;


};

