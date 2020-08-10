#include "Fjord/Graphics/Shader.h" 

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/FileUtil.h" 

namespace Fjord 
{

    static Vector<String> s_Prefixes({"Shaders/"});

    const int ATTRIBUTE_INDEX[(unsigned) Attribute::count] =
    {
        0, 1, 2, 3, 4, 5, 6 
    };

    const char* const ATTRIBUTE_NAME[(unsigned) Attribute::count] =
    {
        "a_Position",
        "a_Normal",
        "a_Tangent",
        "a_Texture0", 
        "a_Texture1", 
        "a_Color0", 
        "a_Color1" 
    };

    int GetAttributeIndex(Attribute attrib) 
    {
        return ATTRIBUTE_INDEX[(int) attrib]; 
    }

    const char* GetAttributeName(Attribute attrib) 
    {
        return ATTRIBUTE_NAME[(int) attrib]; 
    }

    ParameterType GetParameterType(const char* name, GLenum type) 
    {
        switch (type) 
        {
        case GL_INT: return ParameterType::Int; 
        case GL_SAMPLER_2D: return ParameterType::Int; 
        case GL_FLOAT: return ParameterType::Float; 
        case GL_FLOAT_VEC2: return ParameterType::Vector2; 
        case GL_FLOAT_VEC3: return ParameterType::Vector3; 
        case GL_FLOAT_VEC4: return ParameterType::Vector4; 
        case GL_FLOAT_MAT3: return ParameterType::Matrix3; 
        case GL_FLOAT_MAT4: return ParameterType::Matrix4; 
        default: 
            FJ_EFWARN("Unknown Parameter type for '%s'", name); 
            return ParameterType::Unknown; 
        }
    }

    Shader* Shader::Load(const String& file) 
    {
        return new Shader(
            ReadFile(FindPath(file + ".vs", &s_Prefixes)), 
            ReadFile(FindPath(file + ".fs", &s_Prefixes)) 
        );
    }

    Shader::Shader(const String& vs, const String& fs) 
        : VSource_(vs) 
        , FSource_(fs) 
    {
        Create(); 
    }

    Shader::~Shader() 
    {
        Destroy(); 
    }

    void Shader::Update() 
    {
        // if (!Dirty_) return; 

        UploadToGPU(); 

        Dirty_ = false; 
    }

    void Shader::ClearParameter(const String& name) 
    {
        auto it = Parameters_.find(name); 

        if (it != Parameters_.end()) 
        {
            auto& param = it->second; 
            param.Reset();
            FlagUpdateParameter(name); 
        }
    }

    void Shader::SetParameter(const String& name, const Parameter& value) 
    {
        auto it = Parameters_.find(name); 

        if (it != Parameters_.end()) 
        {
            auto& param = it->second; 
            param.Set(value); 
            if (param.Dirty) FlagUpdateParameter(name); 
        }
    }

    void Shader::Create() 
    {
        if (Handle_ != 0) return; 

        GLuint vId, fId; 

        GLCALL(vId = glCreateShader(GL_VERTEX_SHADER)); 
        GLCALL(fId = glCreateShader(GL_FRAGMENT_SHADER)); 
        GLCALL(Handle_ = glCreateProgram()); 

        Valid_ = true; 

        GLCALL(Valid_ &= CompileShader(vId, "vertex shader", VSource_)); 
        GLCALL(Valid_ &= CompileShader(fId, "fragment shader", FSource_)); 
        GLCALL(Valid_ &= LinkProgram(vId, fId)); 

        if (Valid_) FindUniforms(); 

        GLCALL(glDeleteShader(vId)); 
        GLCALL(glDeleteShader(fId)); 
    }

    void Shader::UploadToGPU() 
    {
        // GLCALL(glUseProgram(Handle_)); 
        GetAPI()->BindShader(Handle_); 

        for (auto& name : UpdateParameterSet_) 
        {
            auto it = Parameters_.find(name); 

            if (it != Parameters_.end()) 
            {
                ShaderParameter& param = it->second; 

                if (param.Dirty) // should be true but just check in case 
                {
                    //cout << "Upload parameter: " << param.location << " (" << name << ")" << endl; 
                    switch (param.Type) 
                    {
                    case ParameterType::Int: 
                        // cout << "Upload int uniform" << endl; 
                        GLCALL(glUniform1i(param.Location, param.CurValue.GetInt())); 
                        break; 
                    case ParameterType::Float: 
                        // cout << "Uplaod float uniform" << endl; 
                        GLCALL(glUniform1f(param.Location, param.CurValue.GetFloat())); 
                        break; 
                    case ParameterType::Vector2: 
                        // cout << "Upload vector2 uniform" << endl; 
                        GLCALL(glUniform2fv(param.Location, 1, (float*) &(param.CurValue.GetVector2()))); 
                        break; 
                    case ParameterType::Vector3: 
                        // cout << "Upload vector3 uniform" << endl; 
                        GLCALL(glUniform3fv(param.Location, 1, (float*) &(param.CurValue.GetVector3()))); 
                        break; 
                    case ParameterType::Vector4: 
                        // cout << "Upload vector4 uniform" << endl; 
                        GLCALL(glUniform4fv(param.Location, 1, (float*) &(param.CurValue.GetVector4()))); 
                        break; 
                    case ParameterType::Matrix3: 
                        // cout << "Upload matrix3 uniform" << endl; 
                        GLCALL(glUniformMatrix3fv(param.Location, 1, GL_FALSE, (float*) &(param.CurValue.GetMatrix3()))); 
                        break; 
                    case ParameterType::Matrix4: 
                        // cout << "Upload matrix4 uniform" << endl; 
                        GLCALL(glUniformMatrix4fv(param.Location, 1, GL_FALSE, (float*) &(param.CurValue.GetMatrix4()))); 
                        break; 
                    default: 
                        FJ_EFLOG(Warn, "Unknown parameter type: %d", (int) param.Type); 
                        break; 
                    }

                    param.Dirty = false; 
                }
            }
        }

        UpdateParameterSet_.clear(); 
    } 

    void Shader::Destroy() 
    {
        if (Handle_ != 0) 
        {
            // if (graphics_->GetShader() == this) graphics_->SetShader(nullptr); 

            // GLCALL(glDeleteProgram(Handle_)); 
            GetAPI()->DeleteShader(Handle_); 
            Handle_ = 0; 
        }
    } 

    bool Shader::CompileShader(GLuint id, const char* typeName, const String& source) 
    {
        const char* src = source.c_str();
        int srcLength = source.length();
        GLint status;

        GLCALL(glShaderSource(id, 1, &src, &srcLength));
        GLCALL(glCompileShader(id));

        GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &status));
        if (status != GL_TRUE)
        {
            char text[512];
            GLsizei length;
            GLCALL(glGetShaderInfoLog(id, 512, &length, text));

            ErrorMessage_ += text;
            ErrorMessage_ += "\n";

            FJ_EFLOG(Warn, "Error compiling %s : \n%s", typeName, text); 
            return false;
        }

        FJ_EFLOG(Fine, "Compiling %s success!", typeName); 
        return true;
    }

    bool Shader::LinkProgram(GLuint vId, GLuint fId) 
    {
        GLCALL(glAttachShader(Handle_, vId));
        GLCALL(glAttachShader(Handle_, fId));

        // bind attribs
        for (int i = 0; i < (int) Attribute::count; i++)
        {
            GLCALL(glBindAttribLocation(Handle_, ATTRIBUTE_INDEX[i], ATTRIBUTE_NAME[i]));
            FJ_EFLOG(Fine, "Binding attrib location %d to %s", ATTRIBUTE_INDEX[i], ATTRIBUTE_NAME[i]); 
        }

        GLCALL(glLinkProgram(Handle_));

        GLint status;
        GLCALL(glGetProgramiv(Handle_, GL_LINK_STATUS, &status));
        if (status != GL_TRUE)
        {
            char text[512];
            GLsizei length;
            GLCALL(glGetProgramInfoLog(Handle_, 512, &length, text));

            ErrorMessage_ += text;
            ErrorMessage_ += "\n";

            FJ_EFLOG(Warn, "Error linking program : \n%s", text); 

            GLCALL(glDeleteShader(Handle_));
            Handle_ = 0;

            return false;
        }

        FJ_EFLOG(Fine, "Linking program success!"); 
        return true;
    }

    void Shader::FindUniforms() 
    {
        int count;

        GLCALL(glGetProgramiv(Handle_, GL_ACTIVE_UNIFORMS, &count));

        char name[1024];
        GLsizei length;
        GLint size;
        GLenum type;

        for (int i = 0; i < count; i++)
        {
            GLCALL(glGetActiveUniform(Handle_, i, 1024, &length, &size, &type, name));
            // FJ_EFDEBUG("Found shader parameter: %s", name); 
            ParameterType paramType = GetParameterType(name, type); 
            Parameters_[name] = ShaderParameter(paramType, i, Parameter(paramType));
        }
    }

}