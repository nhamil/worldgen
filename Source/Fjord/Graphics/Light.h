#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Types.h" 
#include "Fjord/Scene/Component.h" 

namespace Fjord 
{

    class Light : public Component 
    {
    public: 
        static const float DefaultRadius; 

        LightType GetType() const; 
        const Color& GetColor() const; 
        float GetRadius() const; 

        void SetType(LightType type); 
        void SetColor(const Color& color); 
        void SetRadius(float radius); 

    private: 
        Color Color_ = Color::White; 
        LightType Type_ = LightType::Point; 
        float Radius_ = DefaultRadius; 
    };

}