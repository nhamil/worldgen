#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector2.h" 
#include "Fjord/Math/Vector3.h" 
#include "Fjord/Math/Vector4.h" 
#include "Fjord/Math/Matrix3.h" 
#include "Fjord/Math/Matrix4.h" 

namespace Fjord 
{

    enum class ParameterType
    {
        Unknown, 
        Int,
        Float,
        Vector2,
        Vector3,
        Vector4,
        Matrix3,
        Matrix4,

        count
    };

    union ParameterValue
    {
        int IntValue; 
        float FloatValue; 
        Vector2 Vec2Value; 
        Vector3 Vec3Value; 
        Vector4 Vec4Value; 
        Matrix3 Mat3Value; 
        Matrix4 Mat4Value; 

        ParameterValue() { IntValue = 0; } 
    };

    class Parameter
    {
    public:
        Parameter(const Parameter& param);
        Parameter();
        Parameter(int value);
        Parameter(float value);
        Parameter(double value);
        Parameter(const Vector2& value);
        Parameter(const Vector3& value);
        Parameter(const Vector4& value);
        Parameter(const Matrix3& value);
        Parameter(const Matrix4& value);
        Parameter(ParameterType type); 
        ~Parameter() {}

        Parameter& operator=(const Parameter& param);
        Parameter& operator=(int value);
        Parameter& operator=(float value);
        Parameter& operator=(double value);
        Parameter& operator=(const Vector2& value);
        Parameter& operator=(const Vector3& value);
        Parameter& operator=(const Vector4& value);
        Parameter& operator=(const Matrix3& value);
        Parameter& operator=(const Matrix4& value);

        bool operator==(const Parameter& param) const;
        bool operator==(int value) const;
        bool operator==(float value) const;
        bool operator==(double value) const;
        bool operator==(const Vector2& value) const;
        bool operator==(const Vector3& value) const;
        bool operator==(const Vector4& value) const;
        bool operator==(const Matrix3& value) const;
        bool operator==(const Matrix4& value) const;

        bool operator!=(const Parameter& param) const { return !(*this == param); }
        bool operator!=(int value) const { return !(*this == value); }
        bool operator!=(float value) const { return !(*this == value); }
        bool operator!=(double value) const { return !(*this == value); }
        bool operator!=(const Vector2& value) const { return !(*this == value); }
        bool operator!=(const Vector3& value) const { return !(*this == value); }
        bool operator!=(const Vector4& value) const { return !(*this == value); }
        bool operator!=(const Matrix3& value) const { return !(*this == value); }
        bool operator!=(const Matrix4& value) const { return !(*this == value); }

        ParameterType GetType() const { return Type_; }

        int GetInt() const;
        float GetFloat() const;
        const Vector2& GetVector2() const;
        const Vector3& GetVector3() const;
        const Vector4& GetVector4() const;
        const Matrix3& GetMatrix3() const;
        const Matrix4& GetMatrix4() const;

    private:
        ParameterType Type_;
        ParameterValue Value_;
    };

}