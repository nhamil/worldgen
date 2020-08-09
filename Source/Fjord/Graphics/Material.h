#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Shader.h" 

namespace Fjord 
{

    class Material : public RefCounted
    {
    public: 
        Material(); 
        virtual ~Material(); 

        void SetShader(Shader* shader); 
        Shader* GetShader() const; 
        void ApplyParameters(); 

        void ClearParameter(const String& name); 
        void SetParameter(const String& name, const Parameter& value); 

        void SetTextureUnit(const String& name, int value) { SetParameter(name, Parameter(value)); } 
        void SetInt(const String& name, int value) { SetParameter(name, Parameter(value)); } 
        void SetFloat(const String& name, float value) { SetParameter(name, Parameter(value)); }  
        void SetVector2(const String& name, const Vector2& value) { SetParameter(name, Parameter(value)); }  
        void SetVector3(const String& name, const Vector3& value) { SetParameter(name, Parameter(value)); }  
        void SetVector4(const String& name, const Vector4& value) { SetParameter(name, Parameter(value)); } 
        void SetMatrix3(const String& name, const Matrix3& value) { SetParameter(name, Parameter(value)); }  
        void SetMatrix4(const String& name, const Matrix4& value) { SetParameter(name, Parameter(value)); }  

    private: 
        HashMap<String, Parameter> Parameters_; 
        Ref<Shader> Shader_; 
    };

}