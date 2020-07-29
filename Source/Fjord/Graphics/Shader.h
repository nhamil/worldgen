#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Parameter.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord
{

    struct ShaderParameter 
    {
        ParameterType Type; 
        unsigned Location; 
        Parameter CurValue; 
        Parameter DefValue; 
        bool Dirty = true; 

        ShaderParameter() = default; 
        ShaderParameter(const ShaderParameter& other) = default; 
        ShaderParameter& operator=(const ShaderParameter& other) = default; 
        ShaderParameter(ParameterType type, unsigned location, const Parameter& value) 
            : Type(type), Location(location), CurValue(value), DefValue(value) {} 

        inline void Reset() 
        {
            if (DefValue != CurValue) 
            {
                CurValue = DefValue; 
                Dirty = true; 
            }
        }

        inline void Set(const Parameter& param) 
        {
            if (param != CurValue) 
            {
                CurValue = param; 
                Dirty = true; 
            }
        }
    };

    class Shader : public GPUObject 
    {
    public: 
        static Shader* Load(const String& file); 

        Shader(const String& vs, const String& fs); 
        virtual ~Shader(); 

        void Update(); 

        bool IsValid() const { return Valid_; } 

        const String& GetErrorMessage() const { return ErrorMessage_; }

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
        void FlagUpdateParameter(const String& name) 
        {
            UpdateParameterSet_.insert(name); 
        }

        void UploadToGPU(); 
        void Create(); 
        void Destroy(); 
        bool CompileShader(unsigned id, const char* typeName, const String& source); 
        bool LinkProgram(unsigned vId, unsigned fId); 
        void FindUniforms(); 

        String VSource_; 
        String FSource_; 
        bool Valid_ = false; 
        bool Dirty_ = true; 
        String ErrorMessage_; 
        HashMap<String, ShaderParameter> Parameters_; 
        HashSet<String> UpdateParameterSet_; 
    };

}