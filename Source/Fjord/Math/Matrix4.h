#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector4.h" 

namespace Fjord 
{

    struct Quaternion;

    /**
     * 4x4 matrix. 
     */ 
    struct Matrix4
    {
        static const Matrix4 Zero;
        static const Matrix4 Identity;

        float M00, M10, M20, M30;
        float M01, M11, M21, M31;
        float M02, M12, M22, M32;
        float M03, M13, M23, M33;

        static Matrix4 Scale(const Vector3& s)
        {
            Matrix4 m;
            m.M00 = s.X;
            m.M11 = s.Y;
            m.M22 = s.Z;
            m.M33 = 1;
            return m;
        }

        static Matrix4 Translation(const Vector3& t)
        {
            Matrix4 m = Identity;
            m.M03 = t.X;
            m.M13 = t.Y;
            m.M23 = t.Z;
            return m;
        }

        static Matrix4 RotationX(float rad)
        {
            Matrix4 m = Identity;
            float c = std::cos(rad);
            float s = std::sin(rad);
            m.M11 = c;
            m.M12 = -s;
            m.M21 = s;
            m.M22 = c;
            return m;
        }

        static Matrix4 RotationY(float rad)
        {
            Matrix4 m = Identity;
            float c = std::cos(rad);
            float s = std::sin(rad);
            m.M00 = c;
            m.M02 = s;
            m.M20 = -s;
            m.M22 = c;
            return m;
        }

        static Matrix4 RotationZ(float rad)
        {
            Matrix4 m = Identity;
            float c = std::cos(rad);
            float s = std::sin(rad);
            m.M00 = c;
            m.M01 = -s;
            m.M10 = s;
            m.M11 = c;
            return m;
        }

        static Matrix4 Perspective(float fov, float ar, float near, float far)
        {
            Matrix4 m;
            fov = 1.0 / std::tan(fov * 0.5);
            m.M00 = fov / ar;
            m.M11 = fov;
            m.M22 = (far + near) / (near - far);
            m.M23 = 2 * far * near / (near - far);
            m.M32 = -1;
            return m;
        }

        static Matrix4 Orthographic(const Vector3& min, const Vector3 max) 
        {
            Matrix4 m = Identity; 
            m.M00 = 2.0f / (max.X - min.X); 
            m.M11 = 2.0f / (max.Y - min.Y); 
            m.M22 = 2.0f / (max.Z - min.Z); 
            m.M03 = -(max.X + min.X) / (max.X - min.X); 
            m.M13 = -(max.Y + min.Y) / (max.Y - min.Y); 
            m.M23 = -(max.Z + min.Z) / (max.Z - min.Z); 
            return m; 
        }

        static Matrix4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::Up)
        {
            Matrix4 m;
            Vector3 f = Normalized(target - eye);
            Vector3 s = Normalized(Cross(f, up));
            Vector3 Y = Normalized(Cross(s, f));
            m.M00 = s.X; m.M01 = s.Y; m.M02 = s.Z;
            m.M10 = Y.X; m.M11 = Y.Y; m.M12 = Y.Z;
            m.M20 = -f.X; m.M21 = -f.Y; m.M22 = -f.Z;
            m.M33 = 1;
            return m *= Translation(-eye);
        }

        static Matrix4 FromQuaternion(const Quaternion& r);

        Matrix4(float M00, float M10, float M20, float M30,
                float M01, float M11, float M21, float M31,
                float M02, float M12, float M22, float M32,
                float M03, float M13, float M23, float M33)
            : M00(M00), M10(M10), M20(M20), M30(M30)
            , M01(M01), M11(M11), M21(M21), M31(M31)
            , M02(M02), M12(M12), M22(M22), M32(M32)
            , M03(M03), M13(M13), M23(M23), M33(M33) {}

        Matrix4(const Vector4& m0, const Vector4& m1, const Vector4& m2, const Vector4& m3)
            : M00(m0.X), M10(m0.Y), M20(m0.Z), M30(m0.W)
            , M01(m1.X), M11(m1.Y), M21(m1.Z), M31(m1.W)
            , M02(m2.X), M12(m2.Y), M22(m2.Z), M32(m2.W)
            , M03(m3.X), M13(m3.Y), M23(m3.Z), M33(m3.W) {}

        explicit Matrix4(float value = 0)
        {
            for (int i = 0; i < 16; i++) (*this)[i] = value;
        }

        static Matrix4 FromArray(const float m[])
        {
            Matrix4 out;
            for (int i = 0; i < 16; i++) out[i] = m[i];
            return out;
        }

        Matrix4(const Matrix4& r)
        {
            for (int i = 0; i < 16; i++) (*this)[i] = r[i];
        }

        bool operator==(const Matrix4& r) const
        {
            for (int i = 0; i < 16; i++) if ((*this)[i] != r[i]) return false;
            return true;
        }
        bool operator!=(const Matrix4& r) const { return !(*this == r); }

        Matrix4& operator=(const Matrix4& r)
        {
            if (this == &r) return *this;

            for (int i = 0; i < 16; i++) (*this)[i] = r[i];

            return *this;
        }

        const Vector4& Column(int index) const { return reinterpret_cast<const Vector4*>(&M00)[index]; }
        Vector4& Column(int index) { return reinterpret_cast<Vector4*>(&M00)[index]; }

        const float& operator[](int index) const { return (&M00)[index]; }
        float& operator[](int index) { return (&M00)[index]; }

        Matrix4& operator+=(float s)
        {
            M00 += s;
            M10 += s;
            M20 += s;
            M30 += s;
            M01 += s;
            M11 += s;
            M21 += s;
            M31 += s;
            M02 += s;
            M12 += s;
            M22 += s;
            M32 += s;
            M03 += s;
            M13 += s;
            M23 += s;
            M33 += s;

            return *this;
        }

        Matrix4& operator-=(float s)
        {
            M00 -= s;
            M10 -= s;
            M20 -= s;
            M30 -= s;
            M01 -= s;
            M11 -= s;
            M21 -= s;
            M31 -= s;
            M02 -= s;
            M12 -= s;
            M22 -= s;
            M32 -= s;
            M03 -= s;
            M13 -= s;
            M23 -= s;
            M33 -= s;

            return *this;
        }

        Matrix4& operator*=(float s)
        {
            M00 *= s;
            M10 *= s;
            M20 *= s;
            M30 *= s;
            M01 *= s;
            M11 *= s;
            M21 *= s;
            M31 *= s;
            M02 *= s;
            M12 *= s;
            M22 *= s;
            M32 *= s;
            M03 *= s;
            M13 *= s;
            M23 *= s;
            M33 *= s;

            return *this;
        }

        Matrix4& operator*=(const Matrix4& r)
        {
            Matrix4 out;

            out.M00 = M00 * r.M00 + M01 * r.M10 + M02 * r.M20 + M03 * r.M30;
            out.M10 = M10 * r.M00 + M11 * r.M10 + M12 * r.M20 + M13 * r.M30;
            out.M20 = M20 * r.M00 + M21 * r.M10 + M22 * r.M20 + M23 * r.M30;
            out.M30 = M30 * r.M00 + M31 * r.M10 + M32 * r.M20 + M33 * r.M30;

            out.M01 = M00 * r.M01 + M01 * r.M11 + M02 * r.M21 + M03 * r.M31;
            out.M11 = M10 * r.M01 + M11 * r.M11 + M12 * r.M21 + M13 * r.M31;
            out.M21 = M20 * r.M01 + M21 * r.M11 + M22 * r.M21 + M23 * r.M31;
            out.M31 = M30 * r.M01 + M31 * r.M11 + M32 * r.M21 + M33 * r.M31;

            out.M02 = M00 * r.M02 + M01 * r.M12 + M02 * r.M22 + M03 * r.M32;
            out.M12 = M10 * r.M02 + M11 * r.M12 + M12 * r.M22 + M13 * r.M32;
            out.M22 = M20 * r.M02 + M21 * r.M12 + M22 * r.M22 + M23 * r.M32;
            out.M32 = M30 * r.M02 + M31 * r.M12 + M32 * r.M22 + M33 * r.M32;

            out.M03 = M00 * r.M03 + M01 * r.M13 + M02 * r.M23 + M03 * r.M33;
            out.M13 = M10 * r.M03 + M11 * r.M13 + M12 * r.M23 + M13 * r.M33;
            out.M23 = M20 * r.M03 + M21 * r.M13 + M22 * r.M23 + M23 * r.M33;
            out.M33 = M30 * r.M03 + M31 * r.M13 + M32 * r.M23 + M33 * r.M33;

            return *this = out;
        }

        Matrix4 operator-() const { return Matrix4(*this) *= -1; }
    };

    inline Vector4 operator*(const Matrix4& a, const Vector4& b) 
    {
        Vector4 out;

        out.X = a.M00 * b.X + a.M01 * b.Y + a.M02 * b.Z + a.M03 * b.W; 
        out.Y = a.M10 * b.X + a.M11 * b.Y + a.M12 * b.Z + a.M13 * b.W; 
        out.Z = a.M20 * b.X + a.M21 * b.Y + a.M22 * b.Z + a.M23 * b.W; 
        out.W = a.M30 * b.X + a.M31 * b.Y + a.M32 * b.Z + a.M33 * b.W; 

        return out;
    }

    inline Vector3 TransformVector(const Matrix4& a, const Vector3& b, bool point) 
    {
        Vector4 out = a * Vector4(b.X, b.Y, b.Z, point ? 1.0 : 0.0); 
        return out.Swizzle3("XYZ") * (point ? 1.0 / out.W : 1.0); 
    }

    inline Matrix4 Transpose(const Matrix4& r) 
    {
        Matrix4 out;
        out.M00 = r.M00;
        out.M01 = r.M10;
        out.M02 = r.M20;
        out.M03 = r.M30;
        out.M10 = r.M01;
        out.M11 = r.M11;
        out.M12 = r.M21;
        out.M13 = r.M31;
        out.M20 = r.M02;
        out.M21 = r.M12;
        out.M22 = r.M22;
        out.M23 = r.M32;
        out.M30 = r.M03;
        out.M31 = r.M13;
        out.M32 = r.M23;
        out.M33 = r.M33;
        return out;
    }

    Matrix4 Inverse(const Matrix4& m); 

    inline Matrix4 operator*(Matrix4 a, const Matrix4& b) { return a *= b; }
    inline Matrix4 operator*(Matrix4 a, float b) { return a *= b; }
    inline Matrix4 operator*(float a, Matrix4 b) { return b *= a; }

    inline Matrix4 operator+(Matrix4 a, float b) { return a += b; }
    inline Matrix4 operator+(float a, Matrix4 b) { return b += a; }

    inline Matrix4 operator-(Matrix4 a, float b) { return a -= b; }
    inline Matrix4 operator-(float a, Matrix4 b) { return -b += a; }

}