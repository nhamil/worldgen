#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Texture2D.h" 

namespace Fjord 
{

    struct Glyph 
    {
        unsigned int Width; 
        unsigned int Height; 
        int BearingX; 
        int BearingY; 
        unsigned int Advance; 
        unsigned int TextureX; 
        unsigned int TextureY; 
    };

    class Font : public RefCounted 
    {
    public: 
        Font(const String& file, float fontSize = 16); 
        ~Font(); 

        Texture2D* GetTexture() const; 

        float GetSize() const { return FontSize_; } 

        const Glyph* GetGlyph(unsigned c) const; 
        int GetKerning(unsigned c1, unsigned c2) const; 

    private: 
        HashMap<unsigned, Glyph> Glyphs_; 
        Ref<Texture2D> Texture_; 
        float FontSize_; 
    };

}