#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector4.h" 

#include <cmath> 

namespace Fjord 
{

    struct Matrix4;

    /**
     * Quaternion implementation. Commonly used for 3D rotations. 
     */ 
    struct Quaternion
    {
        static const Quaternion Identity; 

        float X, Y, Z, W;

        static Quaternion AxisAngle(const Vector3& axis, float radians)
        {
            float s = std::sin(radians * 0.5);
            float c = std::cos(radians * 0.5);

            return Quaternion(s * Normalized(axis), c);
        }

        static Quaternion FromMatrix4(const Matrix4& m);

        static Quaternion Direction(const Vector3& dir, const Vector3& up = Vector3::Up);

        Quaternion() : X(0), Y(0), Z(0), W(0) {}

        explicit Quaternion(float a) : X(a), Y(a), Z(a), W(a) {}

        Quaternion(float a, float b) : X(a), Y(b), Z(0), W(1) {}

        explicit Quaternion(const Vector2& a) : X(a.X), Y(a.Y), Z(0), W(1) {}

        Quaternion(float a, float b, float c) : X(a), Y(b), Z(c), W(1) {}

        Quaternion(float a, float b, float c, float d) : X(a), Y(b), Z(c), W(d) {}

        Quaternion(const Vector2& ab, float c) : X(ab.X), Y(ab.Y), Z(c), W(1) {}

        Quaternion(const Vector2& ab, float c, float d) : X(ab.X), Y(ab.Y), Z(c), W(d) {}

        Quaternion(float a, const Vector2& bc, float d) : X(a), Y(bc.X), Z(bc.Y), W(d) {}

        Quaternion(float a, const Vector2& bc) : X(a), Y(bc.X), Z(bc.Y), W(1) {}

        Quaternion(float a, float b, const Vector2& cd) : X(a), Y(b), Z(cd.X), W(cd.Y) {}

        explicit Quaternion(const Vector3& r) : X(r.X), Y(r.Y), Z(r.Z), W(1) {}

        Quaternion(const Vector3& r, float d) : X(r.X), Y(r.Y), Z(r.Z), W(d) {}

        Quaternion(float a, const Vector3& r) : X(a), Y(r.X), Z(r.Y), W(r.Z) {}

        explicit Quaternion(const Vector4& r) : X(r.X), Y(r.Y), Z(r.Z), W(r.W) {}

        Quaternion(const Quaternion& r) : X(r.X), Y(r.Y), Z(r.Z), W(r.W) {}

        bool operator==(const Quaternion& r) const { return X == r.X && Y == r.Y && Z == r.Z && W == r.W; }
        bool operator!=(const Quaternion& r) const { return !(*this == r); }

        Quaternion& operator=(const Quaternion& r)
        {
            if (this == &r) return *this;
            X = r.X;
            Y = r.Y;
            Z = r.Z;
            W = r.W;
            return *this;
        }

        const float& operator[](int index) const { return (&X)[index]; };
        float& operator[](int index) { return (&X)[index]; };

        Quaternion operator-() const { return Quaternion(-X, -Y, -Z, -W); }

        Quaternion& operator*=(const Quaternion& r)
        {
            Quaternion out;
            out.W = W * r.W - X * r.X - Y * r.Y - Z * r.Z;
            out.X = W * r.X + X * r.W + Y * r.Z - Z * r.Y;
            out.Y = W * r.Y - X * r.Z + Y * r.W + Z * r.X;
            out.Z = W * r.Z + X * r.Y - Y * r.X + Z * r.W;
            return *this = out;
        }
    };

    inline float Length2(const Quaternion& r) { return r.X * r.X + r.Y * r.Y + r.Z * r.Z + r.W * r.W; }
    inline float Length(const Quaternion& r) { return std::sqrt(Length2(r)); }

    inline float Dot(const Quaternion& a, const Quaternion& b) { return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W; }

    inline Quaternion Conjugate(const Quaternion& r) { return Quaternion(-r.X, -r.Y, -r.Z, r.W); }

    inline Quaternion Inverse(const Quaternion& r) { return Conjugate(r) *= Quaternion(1.0 / Length2(r)); }

    inline Quaternion operator*(Quaternion a, const Quaternion& b) { return a *= b; }

    inline Vector3 operator*(Quaternion a, const Vector3& b)
    {
        Quaternion res = a * Quaternion(b, 0) * Conjugate(a);
        return Vector3(res.X, res.Y, res.Z);
    }

    inline Quaternion Normalized(const Quaternion& r) 
    {
        float len = Length(r);

        if (len != 0)
        {
            float inv = 1.0 / len;
            return Quaternion(r.X * inv, r.Y * inv, r.Z * inv, r.W * inv);
        }

        return Quaternion(0);
    }

    inline std::ostream& operator<<(std::ostream& out, const Quaternion& r)
    {
        return out << "(" << r.X << ", " << r.Y << ", " << r.Z << ", " << r.W << ")";
    }

}