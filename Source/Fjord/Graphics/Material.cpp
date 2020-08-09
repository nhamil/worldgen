#include "Fjord/Graphics/Material.h" 

namespace Fjord 
{

    Material::Material() {}

    Material::~Material() {} 

    void Material::SetShader(Shader* shader) 
    {
        Shader_ = shader; 
    }

    Shader* Material::GetShader() const 
    {
        return Shader_; 
    }

    void Material::ApplyParameters() 
    {
        for (auto it : Parameters_) 
        {
            Shader_->SetParameter(it.first, it.second); 
        }
        Shader_->Update(); 
    }

    void Material::ClearParameter(const String& name) 
    {
        auto it = Parameters_.find(name); 
        if (it != Parameters_.end()) 
        {
            Parameters_.erase(it); 
        }
    }

    void Material::SetParameter(const String& name, const Parameter& value) 
    {
        Parameters_[name] = value; 
    }

}