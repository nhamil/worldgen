#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector2.h" 
#include "Fjord/Math/Vector3.h" 

#include <cmath> 

namespace Fjord 
{

    /**
     * 3x3 matrix. 
     */ 
    struct Matrix3 
    {
        static const Matrix3 Zero; 
        static const Matrix3 Identity; 

        float M00, M10, M20; 
        float M01, M11, M21; 
        float M02, M12, M22; 

        static Matrix3 Scale(const Vector2& s)
        {
            Matrix3 m;
            m.M00 = s.X;
            m.M11 = s.Y;
            m.M22 = 1;
            return m;
        }

        static Matrix3 Translation(const Vector2& t)
        {
            Matrix3 m = Identity;
            m.M02 = t.X;
            m.M12 = t.Y;
            return m;
        }

        static Matrix3 Rotation(float rad)
        {
            Matrix3 m = Identity;
            float c = std::cos(rad);
            float s = std::sin(rad);
            m.M00 = c;
            m.M01 = -s;
            m.M10 = s;
            m.M11 = c;
            return m;
        }

        Matrix3(float M00, float M10, float M20,
                float M01, float M11, float M21,
                float M02, float M12, float M22)
            : M00(M00), M10(M10), M20(M20)
            , M01(M01), M11(M11), M21(M21)
            , M02(M02), M12(M12), M22(M22) {}

        Matrix3(const Vector3& m0, const Vector3& m1, const Vector3& m2)
            : M00(m0.X), M10(m0.Y), M20(m0.Z)
            , M01(m1.X), M11(m1.Y), M21(m1.Z)
            , M02(m2.X), M12(m2.Y), M22(m2.Z) {}

        explicit Matrix3(float value = 0)
        {
            for (int i = 0; i < 9; i++) (*this)[i] = value;
        }

        static Matrix3 FromArray(const float m[])
        {
            Matrix3 out;
            for (int i = 0; i < 9; i++) out[i] = m[i];
            return out;
        }

        Matrix3(const Matrix3& r)
        {
            for (int i = 0; i < 9; i++) (*this)[i] = r[i];
        }

        Matrix3& operator=(const Matrix3& r)
        {
            if (this == &r) return *this;

            for (int i = 0; i < 9; i++) (*this)[i] = r[i];

            return *this;
        }

        bool operator==(const Matrix3& r) const
        {
            for (int i = 0; i < 9; i++) if ((*this)[i] != r[i]) return false;
            return true;
        }
        bool operator!=(const Matrix3& r) const { return !(*this == r); }

        const Vector3& Column(int index) const { return reinterpret_cast<const Vector3*>(&M00)[index]; }
        Vector3& Column(int index) { return reinterpret_cast<Vector3*>(&M00)[index]; }

        const float& operator[](int index) const { return (&M00)[index]; }
        float& operator[](int index) { return (&M00)[index]; }

        Matrix3& operator*=(const Matrix3& r)
        {
            Matrix3 out;

            out.M00 = M00 * r.M00 + M01 * r.M10 + M02 * r.M20;
            out.M10 = M10 * r.M00 + M11 * r.M10 + M12 * r.M20;
            out.M20 = M20 * r.M00 + M21 * r.M10 + M22 * r.M20;

            out.M01 = M00 * r.M01 + M01 * r.M11 + M02 * r.M21;
            out.M11 = M10 * r.M01 + M11 * r.M11 + M12 * r.M21;
            out.M21 = M20 * r.M01 + M21 * r.M11 + M22 * r.M21;

            out.M02 = M00 * r.M02 + M01 * r.M12 + M02 * r.M22;
            out.M12 = M10 * r.M02 + M11 * r.M12 + M12 * r.M22;
            out.M22 = M20 * r.M02 + M21 * r.M12 + M22 * r.M22;

            return *this = out;
        }

        Matrix3& operator+=(float s)
        {
            M00 += s;
            M10 += s;
            M20 += s;
            M01 += s;
            M11 += s;
            M21 += s;
            M02 += s;
            M12 += s;
            M22 += s;
            return *this;
        }

        Matrix3& operator-=(float s)
        {
            M00 -= s;
            M10 -= s;
            M20 -= s;
            M01 -= s;
            M11 -= s;
            M21 -= s;
            M02 -= s;
            M12 -= s;
            M22 -= s;
            return *this;
        }

        Matrix3& operator*=(float s)
        {
            M00 *= s;
            M10 *= s;
            M20 *= s;
            M01 *= s;
            M11 *= s;
            M21 *= s;
            M02 *= s;
            M12 *= s;
            M22 *= s;
            return *this;
        }

        Matrix3 operator-() const { return Matrix3(*this) *= -1; }

    }; 

    inline Matrix3 Transpose(const Matrix3& r) 
    {
        Matrix3 out;
        out.M00 = r.M00;
        out.M01 = r.M10;
        out.M02 = r.M20;
        out.M10 = r.M01;
        out.M11 = r.M11;
        out.M12 = r.M21;
        out.M20 = r.M02;
        out.M21 = r.M12;
        out.M22 = r.M22;
        return out;
    }

    inline Matrix3 operator*(Matrix3 a, const Matrix3& b) { return a *= b; }
    inline Matrix3 operator*(Matrix3 a, float b) { return a *= b; }
    inline Matrix3 operator*(float a, Matrix3 b) { return b *= a; }

    inline Matrix3 operator+(Matrix3 a, float b) { return a += b; }
    inline Matrix3 operator+(float a, Matrix3 b) { return b += a; }

    inline Matrix3 operator-(Matrix3 a, float b) { return a -= b; }
    inline Matrix3 operator-(float a, Matrix3 b) { return -b += a; }

}