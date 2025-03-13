#pragma once
#include "Vector.h"

using namespace NCL::Maths;

enum class LightType {
    Point,   // Regular point light
    Spot     // Spotlight
};

class Light {
public:
    Light(const Vector3& pos, const Vector3& dir, const Vector4& col, float rad, float cutoffAngle, LightType type)
        : position(pos), direction(Vector::Normalise(dir)), color(col), radius(rad), cutoff(cutoffAngle), lightType(type) {}

    // Getters
    Vector3 GetPosition() const { return position; }
    Vector3 GetDirection() const { return direction; }
    Vector4 GetColor() const { return color; }
    float GetRadius() const { return radius; }
    float GetCutoff() const { return cutoff; } // Cutoff angle in degrees
    LightType GetLightType() const { return lightType; }

    // Setters
    void SetPosition(const Vector3& pos) { position = pos; }
    void SetDirection(const Vector3& dir) { direction = Vector::Normalise(dir); }
    void SetColor(const Vector4& col) { color = col; }
    void SetRadius(float rad) { radius = rad; }
    void SetCutoff(float angle) { cutoff = angle; } // Set cutoff in degrees
    void SetLightType(LightType type) { lightType = type; }

private:
    Vector3 position;   // Light position
    Vector3 direction;  // Direction spotlight is pointing
    Vector4 color;      // Light color (RGBA)
    float radius;       // Light intensity falloff radius
    float cutoff;       // Spotlight cutoff angle in degrees
    LightType lightType; // New: Light type (Point or Spot)
};
