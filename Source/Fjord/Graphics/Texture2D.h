#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Texture.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class Texture2D : public Texture  
    {
    public: 
        static Texture2D* Load(const String& file); 

        Texture2D(unsigned width, unsigned height, TextureFormat format = TextureFormat::RGBA8); 
        virtual ~Texture2D(); 

        void Update(); 

        virtual TextureType GetType() const override { return TextureType::Texture2D; } 

        unsigned GetWidth() const { return Width_; } 
        unsigned GetHeight() const { return Height_; } 
        float GetInvWidth() const { return InvWidth_; } 
        float GetInvHeight() const { return InvHeight_; } 
        TextureFormat GetFormat() const { return Format_; } 
        TextureFilter GetFilter() const { return Filter_; } 
        AddressMode GetAddressMode() const { return AddressMode_; } 

        void SetData(unsigned x, unsigned y, unsigned width, unsigned height, const void* data); 
        void SetData(const void* data); 

        void SetFilter(TextureFilter filter); 
        void SetAddressMode(AddressMode mode); 

    private: 
        Vector<uint8> Data_; 
        unsigned Width_; 
        unsigned Height_; 
        float InvWidth_; 
        float InvHeight_; 
        TextureFormat Format_; 
        TextureFilter Filter_ = TextureFilter::Trilinear; 
        AddressMode AddressMode_ = AddressMode::Clamp; 
        bool Dirty_ = true; 
        bool DirtyParams_ = true; 
    };

}