#pragma once 

#include "Fjord/Math/MathUtil.h" 
#include "Fjord/Math/Vector3.h" 

namespace Fjord 
{

    inline Vector3 ClampBaryCentricCoords(const Vector3& x) 
    {
        Vector3 out = x; 
        out.X = Max(out.X, 0.0f); 
        out.Y = Max(out.Y, 0.0f); 
        out.Z = Max(out.Z, 0.0f); 
        return out / (out.X + out.Y + out.Z); 
    }

    /**
     * Compute barycentric coordinates of a point given 3 vertices 
     */ 
    inline Vector3 GetBarycentricCoords(
        const Vector3& p, 
        const Vector3& a, 
        const Vector3& b, 
        const Vector3& c, 
        bool clamp = false)
    {
        // from https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
        Vector3 v0 = b - a, v1 = c - a, v2 = p - a, out;
        float d00 = Dot(v0, v0);
        float d01 = Dot(v0, v1);
        float d11 = Dot(v1, v1);
        float d20 = Dot(v2, v0);
        float d21 = Dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;
        out.Y = (d11 * d20 - d01 * d21) / denom;
        out.Z = (d00 * d21 - d01 * d20) / denom;
        out.X = 1.0f - out.Y - out.Z;
        if (clamp) out = ClampBaryCentricCoords(out); 
        return out; 
    }

    template <class T> 
    inline T TriLerp(const T& a, const T& b, const T& c, const Vector3& x) 
    {
        return a * x.X + b * x.Y + c * x.Z; 
    }

}