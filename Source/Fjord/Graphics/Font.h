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

    struct StringMetrics 
    {
        float PxWidth = 0; 
        float PxHeight = 0; 
    };

    class FontFace : public RefCounted 
    {
    public: 
        FontFace(const String& file, float fontSize = 16); 
        ~FontFace(); 

        const String& GetName() const { return FontName_; } 

        Texture2D* GetTexture() const; 

        float GetSize() const { return FontSize_; } 

        const Glyph* GetGlyph(unsigned c) const; 

        StringMetrics GetMetrics(const char* str) const; 
        StringMetrics GetMetrics(const String& str) const 
        {
            return GetMetrics(str.c_str()); 
        }

    private: 
        HashMap<unsigned, Glyph> Glyphs_; 
        Ref<Texture2D> Texture_; 
        float FontSize_{0}; 
        float MaxGlyphHeight_{0}; 
        float GlyphOffsetY_{0}; 
        String FontName_{}; 
    };

    class Font : public RefCounted 
    {
    public: 
        static Font* GetDefaultFont(); 

        static const unsigned DefaultSize = 16; 

        Font(const String& file); 
        ~Font(); 

        const String& GetName() const { return FontName_; } 

        FontFace* GetFace(unsigned size = DefaultSize); 

        StringMetrics GetMetrics(const char* str, unsigned size = DefaultSize) 
        {
            return GetFace(size)->GetMetrics(str); 
        }

        StringMetrics GetMetrics(const String& str, unsigned size = DefaultSize) 
        {
            return GetFace(size)->GetMetrics(str); 
        }

    private: 
        static Ref<Font> DefaultFont_; 

        HashMap<unsigned, Ref<FontFace>> Sizes_{}; 
        String FontName_{}; 
    };

}