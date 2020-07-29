#include "Fjord/Graphics/Font.h" 

#include "Fjord/Util/FileUtil.h" 

#include <ft2build.h> 
#include FT_FREETYPE_H  

#include <cmath> 

namespace Fjord 
{

    static bool s_FreeTypeInit = false; 
    static FT_Library s_FreeType; 

    Font::Font(const String& file, float fontSize) 
        : FontSize_(fontSize) 
    {
        if (!s_FreeTypeInit) 
        {
            if (FT_Init_FreeType(&s_FreeType)) 
            {
                FJ_EFWARN("Could not initialize FreeType"); 
            }
        }

        FT_Face face; 
        if (FT_New_Face(s_FreeType, file.c_str(), 0, &face)) 
        {
            FJ_EFWARN("Could not load font from '%s'", file.c_str()); 
        }

        FT_Set_Pixel_Sizes(face, 0, (int) FontSize_); 

        // uint8* fillData = new uint8[ImageWidth * ImageHeight]; 
        // for (unsigned i = 0; i < ImageWidth*ImageHeight*4; i++) fillData[i] = 0x00; 
        // Texture_->SetData(fillData); 
        // Texture_->Update(); 
        // delete[] fillData; 

        unsigned spacing = 3; 
        unsigned x = spacing, y = spacing; 
        unsigned maxWidth = 1, maxHeight = 1; 
        
        for (uint32 c = 0; c < 256; c++) 
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
            {
                FJ_EFWARN("Could not load glyph for '%c' (%u)", c, (unsigned) c); 
            }

            unsigned width = face->glyph->bitmap.width; 
            unsigned height = face->glyph->bitmap.rows; 

            if (width > maxWidth) maxWidth = width; 
            if (height > maxHeight) maxHeight = height; 
        }

        unsigned numChars = 256; 
        unsigned maxPixels = (
            (spacing + numChars * (maxWidth + spacing)) * 
            (spacing + maxHeight + spacing)
        ); 

        unsigned ImageWidth = (int) std::sqrt((double) maxPixels); 
        unsigned ImageHeight = maxPixels / ImageWidth; 

        FJ_EFDEBUG("Max Size: %ux%u", maxWidth, maxHeight); 
        FJ_EFDEBUG("Font Texture Size: %ux%u (max pixels = %u)", 
            ImageWidth, 
            ImageHeight, 
            maxPixels 
        ); 

        Texture_ = new Texture2D(ImageWidth, ImageHeight, TextureFormat::R8); 

        for (uint32 c = 0; c < 256; c++) 
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
            {
                FJ_EFWARN("Could not load glyph for '%c' (%u)", c, (unsigned) c); 
            }

            unsigned width = face->glyph->bitmap.width; 
            unsigned height = face->glyph->bitmap.rows; 
            const void* data = face->glyph->bitmap.buffer; 

            if (x + width + spacing >= ImageWidth) 
            {
                x = spacing; 
                y += maxHeight + spacing; 
                if (x + width + spacing >= ImageWidth || y + height + spacing >= ImageHeight) 
                {
                    FJ_EFWARN("Not enough texture space to store font (on glyph '%c' (%u))", c, (unsigned) c); 
                    break; 
                }
            }

            Texture_->SetData(x, y, width, height, data); 

            Glyph glyph; 
            glyph.Width = width; 
            glyph.Height = height; 
            glyph.BearingX = face->glyph->bitmap_left; 
            glyph.BearingY = face->glyph->bitmap_top; 
            glyph.Advance = face->glyph->advance.x >> 6; 
            glyph.TextureX = x; 
            glyph.TextureY = y; 
            Glyphs_[c] = glyph; 

            x += width + spacing; 
        }
        Texture_->Update(); 

        FT_Done_Face(face); 
    }

    Font::~Font() 
    {

    }

    Texture2D* Font::GetTexture() const 
    {
        return Texture_; 
    }

    const Glyph* Font::GetGlyph(unsigned c) const 
    {
        auto it = Glyphs_.find(c); 

        if (it != Glyphs_.end()) 
        {
            return &it->second; 
        }
        else 
        {
            return nullptr; 
        }
    }

}
