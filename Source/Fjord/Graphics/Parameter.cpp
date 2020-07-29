#include "Fjord/Graphics/Parameter.h" 

#include <string.h> 

namespace Fjord 
{

    Parameter::Parameter(const Parameter& param)
    {
        *this = param;
    }

    Parameter::Parameter()
    {
        *this = 0;
    }

    Parameter::Parameter(int value)
    {
        *this = value;
    }

    Parameter::Parameter(float value)
    {
        *this = value;
    }

    Parameter::Parameter(double value)
    {
        *this = value;
    }

    Parameter::Parameter(const Vector2& value)
    {
        *this = value;
    }

    Parameter::Parameter(const Vector3& value)
    {
        *this = value;
    }

    Parameter::Parameter(const Vector4& value)
    {
        *this = value;
    }

    Parameter::Parameter(const Matrix3& value)
    {
        *this = value;
    }

    Parameter::Parameter(const Matrix4& value)
    {
        *this = value;
    }

    Parameter::Parameter(ParameterType type) 
    {
        Type_ = type; 

        switch (type) 
        {
        case ParameterType::Int: 
            Value_.IntValue = 0; 
            break; 
        case ParameterType::Float: 
            Value_.FloatValue = 0; 
            break; 
        case ParameterType::Vector2: 
            Value_.Vec2Value = Vector2(0); 
            break; 
        case ParameterType::Vector3: 
            Value_.Vec3Value = Vector3(0); 
            break; 
        case ParameterType::Vector4: 
            Value_.Vec4Value = Vector4(0); 
            break; 
        case ParameterType::Matrix3: 
            Value_.Mat3Value = Matrix3(0); 
            break; 
        case ParameterType::Matrix4: 
            Value_.Mat4Value = Matrix4(0); 
            break; 
        default: 
            type = ParameterType::Unknown; 
            Value_.Mat4Value = Matrix4(0); 
        }
    }

    Parameter& Parameter::operator=(const Parameter& param)
    {
        Type_ = param.Type_;

        switch (Type_)
        {
        case ParameterType::Int:
            Value_.IntValue = param.Value_.IntValue;
            break;
        case ParameterType::Float:
            Value_.FloatValue = param.Value_.FloatValue;
            break;
        case ParameterType::Vector2:
            Value_.Vec2Value = param.Value_.Vec2Value;
            break;
        case ParameterType::Vector3:
            Value_.Vec3Value = param.Value_.Vec3Value;
            break;
        case ParameterType::Vector4:
            Value_.Vec4Value = param.Value_.Vec4Value;
            break;
        case ParameterType::Matrix3:
            Value_.Mat3Value = param.Value_.Mat3Value;
            break;
        case ParameterType::Matrix4:
            Value_.Mat4Value = param.Value_.Mat4Value;
            break;
        case ParameterType::Unknown: 
        default: // should not happen, instead update this if new types are added
            void* in = &Value_; 
            memcpy(in, &param.Value_, sizeof (ParameterValue));
            break;
        }

        return *this;
    }

    Parameter& Parameter::operator=(int value)
    {
        Type_ = ParameterType::Int;
        Value_.IntValue = value;
        return *this;
    }

    Parameter& Parameter::operator=(float value)
    {
        Type_ = ParameterType::Float;
        Value_.FloatValue = value;
        return *this;
    }

    Parameter& Parameter::operator=(double value)
    {
        Type_ = ParameterType::Float;
        Value_.FloatValue = value;
        return *this;
    }

    Parameter& Parameter::operator=(const Vector2& value)
    {
        Type_ = ParameterType::Vector2;
        Value_.Vec2Value = value;
        return *this;
    }

    Parameter& Parameter::operator=(const Vector3& value)
    {
        Type_ = ParameterType::Vector3;
        Value_.Vec3Value = value;
        return *this;
    }

    Parameter& Parameter::operator=(const Vector4& value)
    {
        Type_ = ParameterType::Vector4;
        Value_.Vec4Value = value;
        return *this;
    }

    Parameter& Parameter::operator=(const Matrix3& value)
    {
        Type_ = ParameterType::Matrix3;
        Value_.Mat3Value = value;
        return *this;
    }

    Parameter& Parameter::operator=(const Matrix4& value)
    {
        Type_ = ParameterType::Matrix4;
        Value_.Mat4Value = value;
        return *this;
    }

    bool Parameter::operator==(const Parameter& param) const
    {
        switch (param.Type_)
        {
        case ParameterType::Int: return *this == param.Value_.IntValue;
        case ParameterType::Float: return *this == param.Value_.FloatValue;
        case ParameterType::Vector2: return *this == param.Value_.Vec2Value;
        case ParameterType::Vector3: return *this == param.Value_.Vec3Value;
        case ParameterType::Vector4: return *this == param.Value_.Vec4Value;
        case ParameterType::Matrix3: return *this == param.Value_.Mat3Value;
        case ParameterType::Matrix4: return *this == param.Value_.Mat4Value; 
        default: return memcmp(&Value_, &param.Value_, sizeof (ParameterValue));
        }
    }

    bool Parameter::operator==(int value) const
    {
        return Type_ == ParameterType::Int && Value_.IntValue == value;
    }

    bool Parameter::operator==(float value) const
    {
        return Type_ == ParameterType::Float && Value_.FloatValue == value;
    }

    bool Parameter::operator==(double value) const
    {
        return Type_ == ParameterType::Float && Value_.FloatValue == (float)value;
    }

    bool Parameter::operator==(const Vector2& value) const
    {
        return Type_ == ParameterType::Vector2 && Value_.Vec2Value == value;
    }

    bool Parameter::operator==(const Vector3& value) const
    {
        return Type_ == ParameterType::Vector3 && Value_.Vec3Value == value;
    }

    bool Parameter::operator==(const Vector4& value) const
    {
        return Type_ == ParameterType::Vector4 && Value_.Vec4Value == value;
    }

    bool Parameter::operator==(const Matrix3& value) const
    {
        return Type_ == ParameterType::Matrix3 && Value_.Mat3Value == value;
    }

    bool Parameter::operator==(const Matrix4& value) const
    {
        return Type_ == ParameterType::Matrix4 && Value_.Mat4Value == value;
    }

    int Parameter::GetInt() const
    {
        switch (Type_)
        {
        case ParameterType::Int:
            return Value_.IntValue;
        case ParameterType::Float:
            return (int) Value_.FloatValue;
        default:
            return 0;
        }
    }

    float Parameter::GetFloat() const
    {
        switch (Type_)
        {
        case ParameterType::Int:
            return (float) Value_.IntValue;
        case ParameterType::Float:
            return Value_.FloatValue;
        default:
            return 0.0;
        }
    }

    const Vector2& Parameter::GetVector2() const
    {
        return Type_ == ParameterType::Vector2 ? Value_.Vec2Value : Vector2::Zero;
    }

    const Vector3& Parameter::GetVector3() const
    {
        return Type_ == ParameterType::Vector3 ? Value_.Vec3Value : Vector3::Zero;
    }

    const Vector4& Parameter::GetVector4() const
    {
        return Type_ == ParameterType::Vector4 ? Value_.Vec4Value : Vector4::Zero;
    }

    const Matrix3& Parameter::GetMatrix3() const
    {
        return Type_ == ParameterType::Matrix3 ? Value_.Mat3Value : Matrix3::Identity;
    }

    const Matrix4& Parameter::GetMatrix4() const
    {
        return Type_ == ParameterType::Matrix4 ? Value_.Mat4Value : Matrix4::Identity;
    }

}