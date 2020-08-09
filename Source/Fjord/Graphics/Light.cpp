#include "Fjord/Graphics/Light.h" 

namespace Fjord 
{

    const float Light::DefaultRadius = 100.0f; 

    const Color& Light::GetColor() const 
    {
        return Color_; 
    }

    LightType Light::GetType() const 
    {
        return Type_; 
    }

    void Light::SetType(LightType type) 
    {
        Type_ = type; 
    }

    void Light::SetColor(const Color& color) 
    {
        Color_ = color; 
    }

    float Light::GetRadius() const 
    {
        return Radius_; 
    }

    void Light::SetRadius(float rad) 
    {
        Radius_ = rad; 
    }

}