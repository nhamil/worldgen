#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Texture.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class RenderTexture : public Texture
    {
    public: 
        RenderTexture(unsigned width, unsigned height, TextureFormat format = TextureFormat::RGBA8); 
        virtual ~RenderTexture(); 

        void Update(); 

        virtual TextureType GetType() const override { return TextureType::RenderTexture; } 

        unsigned GetWidth() const { return Width_; } 
        unsigned GetHeight() const { return Height_; } 
        float GetInvWidth() const { return InvWidth_; } 
        float GetInvHeight() const { return InvHeight_; } 
        TextureFormat GetFormat() const { return Format_; } 
        TextureFilter GetFilter() const { return Filter_; } 
        AddressMode GetAddressMode() const { return AddressMode_; } 

        void SetFilter(TextureFilter filter); 
        void SetAddressMode(AddressMode mode); 

    private: 
        unsigned Width_; 
        unsigned Height_; 
        float InvWidth_; 
        float InvHeight_; 
        TextureFormat Format_; 
        TextureFilter Filter_ = TextureFilter::Trilinear; 
        AddressMode AddressMode_ = AddressMode::Clamp; 
        bool DirtyParams_ = true; 
        bool Dirty_ = true; 
    };

}