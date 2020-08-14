#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/Texture.h"

namespace Fjord 
{

    struct ShaderInfo 
    {
        HashMap<String, unsigned> ReservedTextureUnits; 
        unsigned AvailableTextureUnit; 
    };

    class Material : public RefCounted
    {
    public: 
        Material(); 
        virtual ~Material(); 

        void SetShader(Shader* shader); 
        Shader* GetShader() const; 
        void ApplyParameters(const ShaderInfo& info); 

        void ClearParameter(const String& name); 
        void SetParameter(const String& name, const Parameter& value); 
        void SetParameter(const String& name, Texture* value); 

        void SetTexture(const String& name, Texture* value) { SetParameter(name, value); } 
        void SetInt(const String& name, int value) { SetParameter(name, Parameter(value)); } 
        void SetFloat(const String& name, float value) { SetParameter(name, Parameter(value)); }  
        void SetVector2(const String& name, const Vector2& value) { SetParameter(name, Parameter(value)); }  
        void SetVector3(const String& name, const Vector3& value) { SetParameter(name, Parameter(value)); }  
        void SetVector4(const String& name, const Vector4& value) { SetParameter(name, Parameter(value)); } 
        void SetMatrix3(const String& name, const Matrix3& value) { SetParameter(name, Parameter(value)); }  
        void SetMatrix4(const String& name, const Matrix4& value) { SetParameter(name, Parameter(value)); }  

        const Parameter* GetParameter(const String& name) const 
        {
            auto it = Parameters_.find(name); 
            if (it != Parameters_.end()) 
            {
                return &it->second; 
            }
            else 
            {
                return nullptr; 
            }
        }

        Texture* GetTexture(const String& name) const 
        {
            auto it = Textures_.find(name); 
            if (it != Textures_.end()) 
            {
                return it->second; 
            }
            else 
            {
                return nullptr; 
            }
        }

    private: 
        HashMap<String, Ref<Texture>> Textures_; 
        HashMap<String, Parameter> Parameters_; 
        Ref<Shader> Shader_; 
    };

}