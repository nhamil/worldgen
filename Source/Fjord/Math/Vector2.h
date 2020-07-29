#pragma once 

#include "Fjord/Common.h" 

#include <cmath> 
#include <ostream>

namespace Fjord 
{

    /** 
     * 2-dimensional vector. 
     */ 
    struct Vector2 
    {
        static const Vector2 Zero; 
        static const Vector2 Up; 
        static const Vector2 Down; 
        static const Vector2 Left; 
        static const Vector2 Right; 

        float X, Y; 

        Vector2() : X(0), Y(0) {} 

        explicit Vector2(float a) : X(a), Y(a) {} 

        Vector2(float a, float b) : X(a), Y(b) {} 

        Vector2(const Vector2& r) : X(r.X), Y(r.Y) {} 

        Vector2& operator=(const Vector2& r) 
        {
            if (this == &r) return *this; 
            X = r.X; 
            Y = r.Y; 
            return *this;
        }

        bool operator==(const Vector2& r) const { return X == r.X && Y == r.Y; } 
        bool operator!=(const Vector2& r) const { return X != r.X || Y != r.Y; }

        const float& operator[](int index) const { return (&X)[index]; };
        float& operator[](int index) { return (&X)[index]; };

        Vector2 Swizzle2(const char* s) 
        {
            return Vector2((&X)[s[0] - 'X'], (&X)[s[1] - 'X']); 
        }

        Vector2& operator+=(const Vector2& r)
        {
            X += r.X;
            Y += r.Y;
            return *this;
        }

        Vector2& operator-=(const Vector2& r)
        {
            X -= r.X;
            Y -= r.Y;
            return *this;
        }

        Vector2& operator*=(const Vector2& r)
        {
            X *= r.X;
            Y *= r.Y;
            return *this;
        }

        Vector2& operator/=(const Vector2& r)
        {
            X /= r.X;
            Y /= r.Y;
            return *this;
        }

        Vector2& operator+=(float r)
        {
            X += r;
            Y += r;
            return *this;
        }

        Vector2& operator-=(float r)
        {
            X -= r;
            Y -= r;
            return *this;
        }

        Vector2& operator*=(float r)
        {
            X *= r;
            Y *= r;
            return *this;
        }

        Vector2& operator/=(float r)
        {
            X /= r;
            Y /= r;
            return *this;
        }

        Vector2 operator-() const { return Vector2(-X, -Y); }
    };

    inline Vector2 operator+(const Vector2& a, float b) { return {a.X + b, a.Y + b}; } 
    inline Vector2 operator-(const Vector2& a, float b) { return {a.X - b, a.Y - b}; }
    inline Vector2 operator*(const Vector2& a, float b) { return {a.X * b, a.Y * b}; }
    inline Vector2 operator/(const Vector2& a, float b) { return {a.X / b, a.Y / b}; }

    inline Vector2 operator+(float a, const Vector2& b) { return {a + b.X, a + b.Y}; } 
    inline Vector2 operator-(float a, const Vector2& b) { return {a - b.X, a - b.Y}; } 
    inline Vector2 operator*(float a, const Vector2& b) { return {a * b.X, a * b.Y}; } 
    inline Vector2 operator/(float a, const Vector2& b) { return {a / b.X, a / b.Y}; } 

    inline Vector2 operator+(Vector2 a, const Vector2& b) { return a += b; }
    inline Vector2 operator-(Vector2 a, const Vector2& b) { return a -= b; }
    inline Vector2 operator*(Vector2 a, const Vector2& b) { return a *= b; }
    inline Vector2 operator/(Vector2 a, const Vector2& b) { return a /= b; }

    inline float Length2(const Vector2& r) 
    {
        return r.X * r.X + r.Y * r.Y; 
    }

    inline float Length(const Vector2& r) 
    {
        return std::sqrt(r.X * r.X + r.Y * r.Y); 
    }

    inline float Dot(const Vector2& a, const Vector2& b) 
    {
        return a.X * b.X + a.Y * b.Y; 
    }

    inline Vector2 Normalized(const Vector2& r) 
    {
        float len = Length(r); 

        if (len != 0) 
        {
            float inv = 1.0 / len; 
            return Vector2(r.X * inv, r.Y * inv); 
        }

        return Vector2::Zero; 
    }

    inline std::ostream& operator<<(std::ostream& out, const Vector2& r)
    {
        return out << "(" << r.X << ", " << r.Y << ")";
    }

}