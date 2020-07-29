#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/MathUtil.h" 
#include "Fjord/Math/Vector2.h" 

#include <cmath> 
#include <ostream>

namespace Fjord 
{

    /** 
     * 3-dimensional vector. 
     */ 
    struct Vector3 
    {
        static const Vector3 Zero; 
        static const Vector3 Up; 
        static const Vector3 Down; 
        static const Vector3 Left; 
        static const Vector3 Right; 
        static const Vector3 Forward; 
        static const Vector3 Backward; 

        float X, Y, Z; 

        Vector3() : X(0), Y(0), Z(0) {} 

        explicit Vector3(float a) : X(a), Y(a), Z(a) {} 

        Vector3(const Vector2& ab, float c) : X(ab.X), Y(ab.Y), Z(c) {} 

        Vector3(float a, const Vector2& bc) : X(a), Y(bc.X), Z(bc.Y) {} 

        Vector3(float a, float b, float c) : X(a), Y(b), Z(c) {} 

        Vector3(const Vector3& r) : X(r.X), Y(r.Y), Z(r.Z) {} 

        Vector3& operator=(const Vector3& r) 
        {
            if (this == &r) return *this; 
            X = r.X; 
            Y = r.Y; 
            Z = r.Z; 
            return *this;
        }

        bool operator==(const Vector3& r) const { return X == r.X && Y == r.Y && Z == r.Z; } 
        bool operator!=(const Vector3& r) const { return X != r.X || Y != r.Y || Z != r.Z; }

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

        Vector3& operator+=(const Vector3& r)
        {
            X += r.X;
            Y += r.Y;
            Z += r.Z; 
            return *this;
        }

        Vector3& operator-=(const Vector3& r)
        {
            X -= r.X;
            Y -= r.Y;
            Z -= r.Z; 
            return *this;
        }

        Vector3& operator*=(const Vector3& r)
        {
            X *= r.X;
            Y *= r.Y;
            Z *= r.Z; 
            return *this;
        }

        Vector3& operator/=(const Vector3& r)
        {
            X /= r.X;
            Y /= r.Y;
            Z /= r.Z; 
            return *this;
        }

        Vector3& operator+=(float r)
        {
            X += r;
            Y += r;
            Z += r; 
            return *this;
        }

        Vector3& operator-=(float r)
        {
            X -= r;
            Y -= r;
            Z -= r; 
            return *this;
        }

        Vector3& operator*=(float r)
        {
            X *= r;
            Y *= r;
            Z *= r; 
            return *this;
        }

        Vector3& operator/=(float r)
        {
            X /= r;
            Y /= r;
            Z /= r; 
            return *this;
        }

        Vector3 operator-() const { return Vector3(-X, -Y, -Z); }
    };

    inline Vector3 operator+(const Vector3& a, float b) { return {a.X + b, a.Y + b, a.Z + b}; } 
    inline Vector3 operator-(const Vector3& a, float b) { return {a.X - b, a.Y - b, a.Z - b}; }
    inline Vector3 operator*(const Vector3& a, float b) { return {a.X * b, a.Y * b, a.Z * b}; }
    inline Vector3 operator/(const Vector3& a, float b) { return {a.X / b, a.Y / b, a.Z / b}; }

    inline Vector3 operator+(float a, const Vector3& b) { return {a + b.X, a + b.Y, a + b.Z}; } 
    inline Vector3 operator-(float a, const Vector3& b) { return {a - b.X, a - b.Y, a - b.Z}; } 
    inline Vector3 operator*(float a, const Vector3& b) { return {a * b.X, a * b.Y, a * b.Z}; } 
    inline Vector3 operator/(float a, const Vector3& b) { return {a / b.X, a / b.Y, a / b.Z}; } 

    inline Vector3 operator+(Vector3 a, const Vector3& b) { return a += b; }
    inline Vector3 operator-(Vector3 a, const Vector3& b) { return a -= b; }
    inline Vector3 operator*(Vector3 a, const Vector3& b) { return a *= b; }
    inline Vector3 operator/(Vector3 a, const Vector3& b) { return a /= b; }

    inline float Length2(const Vector3& r) 
    {
        return r.X * r.X + r.Y * r.Y + r.Z * r.Z; 
    }

    inline float Length(const Vector3& r) 
    {
        return std::sqrt(r.X * r.X + r.Y * r.Y + r.Z * r.Z); 
    }

    inline float Dot(const Vector3& a, const Vector3& b) 
    {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z; 
    }

    inline Vector3 Cross(const Vector3& a, const Vector3& b) 
    {
        return Vector3(
            a.Y * b.Z - a.Z * b.Y, 
            a.Z * b.X - a.X * b.Z, 
            a.X * b.Y - a.Y * b.X 
        );
    }

    inline Vector3 Normalized(const Vector3& r) 
    {
        float len = Length(r); 

        if (len != 0) 
        {
            float inv = 1.0 / len; 
            return Vector3(r.X * inv, r.Y * inv, r.Z * inv); 
        }

        return Vector3::Zero; 
    }

    inline float Angle(const Vector3& a, const Vector3& b) 
    {
        return std::acos(Clamp(Dot(Normalized(a), Normalized(b)), -1, 1)); 
    }

    inline float AngleUnsafe(const Vector3& aNorm, const Vector3& bNorm) 
    {
        return std::acos(Clamp(Dot(aNorm, bNorm), -1, 1)); 
    }

    inline std::ostream& operator<<(std::ostream& out, const Vector3& r)
    {
        return out << "(" << r.X << ", " << r.Y << ", " << r.Z << ")";
    }

}