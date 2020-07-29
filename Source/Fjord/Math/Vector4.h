#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector2.h" 
#include "Fjord/Math/Vector3.h" 

#include <cmath> 
#include <ostream>

namespace Fjord 
{

    /** 
     * 4-dimensional vector. 
     */ 
    struct Vector4 
    {
        static const Vector4 Zero; 

        float X, Y, Z, W; 

        Vector4() : X(0), Y(0), Z(0), W(0) {} 

        explicit Vector4(float a) : X(a), Y(a), Z(a), W(a) {} 

        Vector4(const Vector3& abc, float d) : X(abc.X), Y(abc.Y), Z(abc.Z), W(d) {} 

        Vector4(float a, const Vector3& bcd) : X(a), Y(bcd.X), Z(bcd.Y), W(bcd.Z) {} 

        Vector4(const Vector2& ab, const Vector2& cd) : X(ab.X), Y(ab.Y), Z(cd.X), W(cd.Y) {} 

        Vector4(const Vector2& ab, float c, float d) : X(ab.X), Y(ab.Y), Z(c), W(d) {} 

        Vector4(float a, const Vector2& bc, float d) : X(a), Y(bc.X), Z(bc.Y), W(d) {} 

        Vector4(float a, float b, const Vector2& cd) : X(a), Y(b), Z(cd.X), W(cd.Y) {} 

        Vector4(float a, float b, float c, float d) : X(a), Y(b), Z(c), W(d) {} 

        Vector4(const Vector4& r) : X(r.X), Y(r.Y), Z(r.Z), W(r.W) {} 

        Vector4& operator=(const Vector4& r) 
        {
            if (this == &r) return *this; 
            X = r.X; 
            Y = r.Y; 
            Z = r.Z; 
            W = r.W; 
            return *this;
        }

        bool operator==(const Vector4& r) const { return X == r.X && Y == r.Y && Z == r.Z && W == r.W; } 
        bool operator!=(const Vector4& r) const { return X != r.X || Y != r.Y || Z != r.Z || W != r.W; }

        const float& operator[](int index) const { return (&X)[index]; };
        float& operator[](int index) { return (&X)[index]; };

        Vector2 Swizzle2(const char* s) 
        {
            return Vector2((&X)[s[0] - 'X'], (&X)[s[1] - 'X']); 
        }

        Vector3 Swizzle3(const char* s) 
        {
            return Vector3((&X)[s[0] - 'X'], (&X)[s[1] - 'X'], (&X)[s[2] - 'X']); 
        }

        Vector4 Swizzle4(const char* s) 
        {
            return Vector4((&X)[s[0] - 'X'], (&X)[s[1] - 'X'], (&X)[s[2] - 'X'], (&X)[s[3] - 'X']); 
        }

        Vector4& operator+=(const Vector4& r)
        {
            X += r.X;
            Y += r.Y;
            Z += r.Z; 
            W += r.W; 
            return *this;
        }

        Vector4& operator-=(const Vector4& r)
        {
            X -= r.X;
            Y -= r.Y;
            Z -= r.Z; 
            W -= r.W; 
            return *this;
        }

        Vector4& operator*=(const Vector4& r)
        {
            X *= r.X;
            Y *= r.Y;
            Z *= r.Z; 
            W *= r.W; 
            return *this;
        }

        Vector4& operator/=(const Vector4& r)
        {
            X /= r.X;
            Y /= r.Y;
            Z /= r.Z; 
            W /= r.W; 
            return *this;
        }

        Vector4& operator+=(float r)
        {
            X += r;
            Y += r;
            Z += r; 
            W += r; 
            return *this;
        }

        Vector4& operator-=(float r)
        {
            X -= r;
            Y -= r;
            Z -= r; 
            W -= r; 
            return *this;
        }

        Vector4& operator*=(float r)
        {
            X *= r;
            Y *= r;
            Z *= r; 
            W *= r; 
            return *this;
        }

        Vector4& operator/=(float r)
        {
            X /= r;
            Y /= r;
            Z /= r; 
            W /= r; 
            return *this;
        }

        Vector4 operator-() const { return Vector4(-X, -Y, -Z, -W); }
    };

    inline Vector4 operator+(const Vector4& a, float b) { return {a.X + b, a.Y + b, a.Z + b, a.W + b}; } 
    inline Vector4 operator-(const Vector4& a, float b) { return {a.X - b, a.Y - b, a.Z - b, a.W - b}; }
    inline Vector4 operator*(const Vector4& a, float b) { return {a.X * b, a.Y * b, a.Z * b, a.W * b}; }
    inline Vector4 operator/(const Vector4& a, float b) { return {a.X / b, a.Y / b, a.Z / b, a.W / b}; }

    inline Vector4 operator+(float a, const Vector4& b) { return {a + b.X, a + b.Y, a + b.Z, a + b.W}; } 
    inline Vector4 operator-(float a, const Vector4& b) { return {a - b.X, a - b.Y, a - b.Z, a - b.W}; } 
    inline Vector4 operator*(float a, const Vector4& b) { return {a * b.X, a * b.Y, a * b.Z, a * b.W}; } 
    inline Vector4 operator/(float a, const Vector4& b) { return {a / b.X, a / b.Y, a / b.Z, a / b.W}; } 

    inline Vector4 operator+(Vector4 a, const Vector4& b) { return a += b; }
    inline Vector4 operator-(Vector4 a, const Vector4& b) { return a -= b; }
    inline Vector4 operator*(Vector4 a, const Vector4& b) { return a *= b; }
    inline Vector4 operator/(Vector4 a, const Vector4& b) { return a /= b; }

    inline float Length2(const Vector4& r) 
    {
        return r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W; 
    }

    inline float Length(const Vector4& r) 
    {
        return std::sqrt(r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W); 
    }

    inline float Dot(const Vector4& a, const Vector4& b) 
    {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W; 
    }

    inline Vector4 Normalized(const Vector4& r) 
    {
        float len = Length(r); 

        if (len != 0) 
        {
            float inv = 1.0 / len; 
            return Vector4(r.X * inv, r.Y * inv, r.Z * inv, r.W * inv); 
        }

        return Vector4::Zero; 
    }

    inline std::ostream& operator<<(std::ostream& out, const Vector4& r)
    {
        return out << "(" << r.X << ", " << r.Y << ", " << r.Z << ", " << r.W << ")";
    }

}