#include "Fjord/Math/Matrix3.h" 
#include "Fjord/Math/Matrix4.h" 
#include "Fjord/Math/Quaternion.h" 
#include "Fjord/Math/Vector2.h" 
#include "Fjord/Math/Vector3.h" 
#include "Fjord/Math/Vector4.h" 

namespace Fjord 
{

    const Vector2 Vector2::Zero(0, 0); 
    const Vector2 Vector2::Up(0, 1); 
    const Vector2 Vector2::Down(0, -1); 
    const Vector2 Vector2::Left(-1, 0); 
    const Vector2 Vector2::Right(1, 0); 

    const Vector3 Vector3::Zero(0, 0, 0); 
    const Vector3 Vector3::Up(0, 1, 0); 
    const Vector3 Vector3::Down(0, -1, 0); 
    const Vector3 Vector3::Left(-1, 0, 0); 
    const Vector3 Vector3::Right(1, 0, 0); 
    const Vector3 Vector3::Forward(0, 0, -1); 
    const Vector3 Vector3::Backward(0, 0, 1); 

    const Vector4 Vector4::Zero(0, 0, 0, 0); 

    const Quaternion Quaternion::Identity = Quaternion::AxisAngle(Vector3::Up, 0); 

    const Matrix3 Matrix3::Zero(0); 
    const Matrix3 Matrix3::Identity(
        1, 0, 0, 
        0, 1, 0, 
        0, 0, 1
    ); 

    const Matrix4 Matrix4::Zero(0); 
    const Matrix4 Matrix4::Identity(
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, 0, 1
    );

    /**
     * Inverse matrx4
     * modified from https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
     */
    Matrix4 Inverse(const Matrix4& m) 
    {
        float inv[16];
        float det;

        inv[0] = m[5]  * m[10] * m[15] -
                m[5]  * m[11] * m[14] -
                m[9]  * m[6]  * m[15] +
                m[9]  * m[7]  * m[14] +
                m[13] * m[6]  * m[11] -
                m[13] * m[7]  * m[10];

        inv[4] = -m[4]  * m[10] * m[15] +
                    m[4]  * m[11] * m[14] +
                    m[8]  * m[6]  * m[15] -
                    m[8]  * m[7]  * m[14] -
                    m[12] * m[6]  * m[11] +
                    m[12] * m[7]  * m[10];

        inv[8] = m[4]  * m[9] * m[15] -
                    m[4]  * m[11] * m[13] -
                    m[8]  * m[5] * m[15] +
                    m[8]  * m[7] * m[13] +
                    m[12] * m[5] * m[11] -
                    m[12] * m[7] * m[9];

        inv[12] = -m[4]  * m[9] * m[14] +
                    m[4]  * m[10] * m[13] +
                    m[8]  * m[5] * m[14] -
                    m[8]  * m[6] * m[13] -
                    m[12] * m[5] * m[10] +
                    m[12] * m[6] * m[9];

        inv[1] = -m[1]  * m[10] * m[15] +
                    m[1]  * m[11] * m[14] +
                    m[9]  * m[2] * m[15] -
                    m[9]  * m[3] * m[14] -
                    m[13] * m[2] * m[11] +
                    m[13] * m[3] * m[10];

        inv[5] = m[0]  * m[10] * m[15] -
                    m[0]  * m[11] * m[14] -
                    m[8]  * m[2] * m[15] +
                    m[8]  * m[3] * m[14] +
                    m[12] * m[2] * m[11] -
                    m[12] * m[3] * m[10];

        inv[9] = -m[0]  * m[9] * m[15] +
                    m[0]  * m[11] * m[13] +
                    m[8]  * m[1] * m[15] -
                    m[8]  * m[3] * m[13] -
                    m[12] * m[1] * m[11] +
                    m[12] * m[3] * m[9];

        inv[13] = m[0]  * m[9] * m[14] -
                    m[0]  * m[10] * m[13] -
                    m[8]  * m[1] * m[14] +
                    m[8]  * m[2] * m[13] +
                    m[12] * m[1] * m[10] -
                    m[12] * m[2] * m[9];

        inv[2] = m[1]  * m[6] * m[15] -
                    m[1]  * m[7] * m[14] -
                    m[5]  * m[2] * m[15] +
                    m[5]  * m[3] * m[14] +
                    m[13] * m[2] * m[7] -
                    m[13] * m[3] * m[6];

        inv[6] = -m[0]  * m[6] * m[15] +
                    m[0]  * m[7] * m[14] +
                    m[4]  * m[2] * m[15] -
                    m[4]  * m[3] * m[14] -
                    m[12] * m[2] * m[7] +
                    m[12] * m[3] * m[6];

        inv[10] = m[0]  * m[5] * m[15] -
                    m[0]  * m[7] * m[13] -
                    m[4]  * m[1] * m[15] +
                    m[4]  * m[3] * m[13] +
                    m[12] * m[1] * m[7] -
                    m[12] * m[3] * m[5];

        inv[14] = -m[0]  * m[5] * m[14] +
                    m[0]  * m[6] * m[13] +
                    m[4]  * m[1] * m[14] -
                    m[4]  * m[2] * m[13] -
                    m[12] * m[1] * m[6] +
                    m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] +
                    m[1] * m[7] * m[10] +
                    m[5] * m[2] * m[11] -
                    m[5] * m[3] * m[10] -
                    m[9] * m[2] * m[7] +
                    m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] -
                    m[0] * m[7] * m[10] -
                    m[4] * m[2] * m[11] +
                    m[4] * m[3] * m[10] +
                    m[8] * m[2] * m[7] -
                    m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] +
                    m[0] * m[7] * m[9] +
                    m[4] * m[1] * m[11] -
                    m[4] * m[3] * m[9] -
                    m[8] * m[1] * m[7] +
                    m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] -
                    m[0] * m[6] * m[9] -
                    m[4] * m[1] * m[10] +
                    m[4] * m[2] * m[9] +
                    m[8] * m[1] * m[6] -
                    m[8] * m[2] * m[5];

        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (det == 0) return Matrix4::Zero; 

        det = 1.0 / det;

        for (int i = 0; i < 16; i++) inv[i] *= det;

        return Matrix4::FromArray(inv);
    }

    Matrix4 Matrix4::FromQuaternion(const Quaternion& r)
    {
        Matrix4 out;
        out.M00 = 1 - 2 * r.Y * r.Y - 2 * r.Z * r.Z;
        out.M01 = 2 * r.X * r.Y - 2 * r.Z * r.W;
        out.M02 = 2 * r.X * r.Z + 2 * r.Y * r.W;
        out.M10 = 2 * r.X * r.Y + 2 * r.Z * r.W;
        out.M11 = 1 - 2 * r.X * r.X - 2 * r.Z * r.Z;
        out.M12 = 2 * r.Y * r.Z - 2 * r.X * r.W;
        out.M20 = 2 * r.X * r.Z - 2 * r.Y * r.W;
        out.M21 = 2 * r.Y * r.Z + 2 * r.X * r.W;
        out.M22 = 1 - 2 * r.X * r.X - 2 * r.Y * r.Y;
        out.M33 = 1;
        return out;
    }

    Quaternion Quaternion::FromMatrix4(const Matrix4& m)
    {
        Quaternion q;

        float t = 1 + m[0] + m[5] + m[10];

        if (t > 0.001) {
            float s = 1.0 / (2 * std::sqrt(t));
            q.X = (m[9] - m[6]) * s;
            q.Y = (m[2] - m[8]) * s;
            q.Z = (m[4] - m[1]) * s;
            q.W = 0.25 / s;
            return q;
        }

        if (m[0] > m[5] && m[0] > m[10]) {
            float s = 1.0 / (2 * std::sqrt(1 + m[0] - m[5] - m[10]));
            q.X = 0.25 / s;
            q.Y = (m[4] + m[1]) * s;
            q.Z = (m[2] + m[8]) * s;
            q.W = (m[9] - m[6]) * s;
            return q;
        }
        else if (m[5] > m[10]) {
            float s = 1.0 / (2 * std::sqrt(1 + m[5] - m[0] - m[10]));
            q.X = (m[4] + m[1]) * s;
            q.Y = 0.25 / s;
            q.Z = (m[9] + m[6]) * s;
            q.W = (m[2] - m[8]) * s;
            return q;
        }
        else {
            float s = 1.0 / (2 * std::sqrt(1 + m[10] - m[0] - m[5]));
            q.X = (m[2] + m[8]) * s;
            q.Y = (m[9] + m[6]) * s;
            q.Z = 0.25 / s;
            q.W = (m[4] - m[1]) * s;
            return q;
        }
    }

    Quaternion Quaternion::Direction(const Vector3& dir, const Vector3& up)
    {
        return FromMatrix4(Matrix4::LookAt(Vector3::Zero, dir, up));
    }

}