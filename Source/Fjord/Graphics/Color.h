#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector4.h" 

namespace Fjord 
{

    struct Color 
    {
        static const Color Black; 
        static const Color White; 
        static const Color Red; 
        static const Color Green; 
        static const Color Blue; 
        static const Color Yellow; 
        static const Color Fuchsia; 
        static const Color Cyan; 

        Color() 
            : R(0), G(0), B(0), A(0) {} 
        Color(const Vector4& r) 
            : R(r.X), G(r.Y), B(r.Z), A(r.W) {}
        Color(float r, float g, float b, float a) 
            : R(r), G(g), B(b), A(a) {}
        Color(float r, float g, float b) 
            : Color(r, g, b, 1.0) {} 

        operator Vector4() const { return Vector4(R, G, B, A); } 

        float R, G, B, A; 
    }; 

}