#include "Fjord/Graphics/Material.h" 

#include "Fjord/Graphics/Graphics.h" 

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

    void Material::ApplyParameters(const ShaderInfo& info) 
    {
        for (auto it : Parameters_) 
        {
            Shader_->SetParameter(it.first, it.second); 
        }

        auto* graphics = GetGraphics(); 

        unsigned texUnit = info.AvailableTextureUnit; 
        for (auto it : Textures_) 
        {
            auto texIt = info.ReservedTextureUnits.find(it.first); 
            if (texIt == info.ReservedTextureUnits.end()) 
            {
                graphics->SetTexture(texUnit, it.second); 
                Shader_->SetTextureUnit(it.first, texUnit++);
            }
            else 
            {
                Shader_->SetTextureUnit(it.first, texIt->second); 
                graphics->SetTexture(texIt->second, it.second); 
            }
        }
        Shader_->Update(); 
    }

    void Material::ClearParameter(const String& name) 
    {
        {
            auto it = Parameters_.find(name); 
            if (it != Parameters_.end()) 
            {
                Parameters_.erase(it); 
            }
        }
        {
            auto it = Textures_.find(name); 
            if (it != Textures_.end()) 
            {
                Textures_.erase(it); 
            }
        }
    }

    void Material::SetParameter(const String& name, const Parameter& value) 
    {
        ClearParameter(name); 
        Parameters_[name] = value; 
    }

    void Material::SetParameter(const String& name, Texture* value) 
    {
        ClearParameter(name); 
        Textures_[name] = value; 
    }

}