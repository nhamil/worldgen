#include "Fjord/Graphics/Texture2D.h"

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/FileUtil.h" 
#include "Fjord/Util/Memory.h" 

#define STB_IMAGE_IMPLEMENTATION 
#include "Fjord/Util/Internal/stb_image.h" 

namespace Fjord 
{

    GLenum GetGLAddressMode(AddressMode mode) 
    {
        switch (mode) 
        {
            case AddressMode::Clamp: return GL_CLAMP_TO_EDGE; 
            case AddressMode::Repeat: return GL_REPEAT; 
            default: 
                FJ_EFWARN("Unsupported address mode: %d", mode); 
                return GL_CLAMP_TO_EDGE;  
        }
    }

    GLenum GetGLMinFilter(TextureFilter filter) 
    {
        switch (filter) 
        {
            case TextureFilter::Nearest: return GL_NEAREST_MIPMAP_NEAREST; 
            case TextureFilter::Linear: return GL_NEAREST_MIPMAP_LINEAR; 
            case TextureFilter::Bilinear: return GL_LINEAR_MIPMAP_NEAREST; 
            case TextureFilter::Trilinear: return GL_LINEAR_MIPMAP_LINEAR; 
            default: 
                FJ_EFWARN("Unsupported min filter: %d", filter); 
                return GL_NEAREST; 
        }
    }

    GLenum GetGLMagFilter(TextureFilter filter) 
    {
        switch (filter) 
        {
            case TextureFilter::Nearest: return GL_NEAREST; 
            case TextureFilter::Linear: return GL_NEAREST; 
            case TextureFilter::Bilinear: return GL_LINEAR; 
            case TextureFilter::Trilinear: return GL_LINEAR; 
            default: 
                FJ_EFWARN("Unsupported mag filter: %d", filter); 
                return GL_NEAREST; 
        }
    }

    GLenum GetGLTextureInternalFormat(TextureFormat format) 
    {
        switch (format) 
        {
            case TextureFormat::RGBA8: return GL_RGBA8; 
            case TextureFormat::RGB8: return GL_RGB8; 
            case TextureFormat::RG8: return GL_RG8; 
            case TextureFormat::R8: return GL_R8; 
            default: 
                FJ_EFWARN("Unsupported internal texture format: %d", format); 
                return GL_R8; 
        }
    }

    GLenum GetGLTextureInputFormat(TextureFormat format) 
    {
        switch (format) 
        {
            case TextureFormat::RGBA8: return GL_RGBA; 
            case TextureFormat::RGB8: return GL_RGB; 
            case TextureFormat::RG8: return GL_RG; 
            case TextureFormat::R8: return GL_RED; 
            default: 
                FJ_EFWARN("Unsupported input texture format: %d", format); 
                return GL_R; 
        }
    }

    GLenum GetGLTextureInputType(TextureFormat format) 
    {
        switch (format) 
        {
            case TextureFormat::RGBA8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::RGB8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::RG8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::R8: return GL_UNSIGNED_BYTE; 
            default: 
                FJ_EFWARN("Unsupported input texture type: %d", format); 
                return GL_UNSIGNED_BYTE; 
        }
    }

    static Vector<String> s_Prefixes({"Textures/"}); 
    static Vector<String> s_Suffixes({".png", ".jpg", ".jpeg"}); 

    Texture2D* Texture2D::Load(const String& filename) 
    {
        String file = FindPath(filename, &s_Prefixes, &s_Suffixes); 
        int width, height, channels; 
        uint8* img = stbi_load(file.c_str(), &width, &height, &channels, 4); 

        Texture2D* tex = nullptr; 

        if (img == nullptr) 
        {
            FJ_EFWARN("Could not load texture from %s", file.c_str()); 
            tex = new Texture2D(1, 1); 
            uint8 fuschia[] = { 0xFF, 0x00, 0xFF, 0xFF }; 
            tex->SetData(fuschia); 
            tex->Update(); 
        }
        else 
        {
            tex = new Texture2D(width, height); 
            tex->SetData(img); 
            tex->Update(); 
        }

        stbi_image_free(img); 

        return tex; 
    }

    Texture2D::Texture2D(unsigned width, unsigned height, TextureFormat format) 
        : Width_(width) 
        , Height_(height) 
        , InvWidth_(1.0 / width) 
        , InvHeight_(1.0 / height) 
        , Format_(format) 
    {
        Data_.resize(Width_ * Height_ * GetTextureFormatByteCount(Format_)); 
        GLCALL(glGenTextures(1, &Handle_)); 
    }

    Texture2D::~Texture2D() 
    {
        if (Handle_ != 0) 
        {
            GLCALL(glDeleteTextures(1, &Handle_)); 
            Handle_ = 0; 
        }
    }

    void Texture2D::Update() 
    {
        if (!Dirty_ && !DirtyParams_) return; 

        GLCALL(glBindTexture(GL_TEXTURE_2D, Handle_)); 

        // TODO needed? 
        GLCALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); 

        if (DirtyParams_) 
        {
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0)); 
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0)); 
            // GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps_ - 1)); 

            GLenum wrapX, wrapY; 
            wrapX = wrapY = GetGLAddressMode(AddressMode_); 

            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX));	
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY)); 

            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLMinFilter(Filter_)));
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLMagFilter(Filter_)));
        }

        if (Dirty_) 
        {
            GLCALL(glTexImage2D(
                GL_TEXTURE_2D, 
                0, // level 
                GetGLTextureInternalFormat(Format_), 
                Width_, 
                Height_, 
                0, // border 
                GetGLTextureInputFormat(Format_), 
                GetGLTextureInputType(Format_), 
                &Data_[0]
            )); 
        }

        // TODO do parameters affect mipmaps? 
        // GLCALL(glGenerateMipmap(GL_TEXTURE_2D)); 

        GLCALL(glBindTexture(GL_TEXTURE_2D, 0)); 

        Dirty_ = DirtyParams_ = false; 
    }

    void Texture2D::SetData(unsigned startx, unsigned starty, unsigned width, unsigned height, const void* data) 
    {
        FJ_EASSERT(startx + width <= Width_); 
        FJ_EASSERT(starty + height <= Height_); 

        Dirty_ = true; 

        uint8* pixels = (uint8*) data; 

        unsigned pxSize = GetTextureFormatByteCount(Format_); 

        for (unsigned y = 0; y < height; y++) 
        {
            unsigned startIndex = (startx + (y + starty) * Width_) * pxSize; 
            unsigned outIndex = y * width * pxSize; 
            unsigned size = width * pxSize; 

            // FJ_EFDEBUG("y = %u", y); 
            // FJ_EFDEBUG("startIndex = %u, outIndex = %u, size = %u", startIndex, outIndex, size); 

            MemCpy(&Data_[startIndex], &pixels[outIndex], size, 1); 
        }

        // for (unsigned y = 0; y < height; y++) 
        // {
        //     for (unsigned x = 0; x < width; x++) 
        //     {
        //         unsigned xx = startx + x; 
        //         unsigned yy = starty + y; 

        //         FJ_EFDEBUG("(%u, %u) = (%u, %u, %u, %u), (%u, %u, %u, %u)", 
        //             xx, yy, 
        //             Data_[(xx + yy * Width_) * 4 + 0], 
        //             Data_[(xx + yy * Width_) * 4 + 1], 
        //             Data_[(xx + yy * Width_) * 4 + 2], 
        //             Data_[(xx + yy * Width_) * 4 + 3], 
        //             pixels[(x + y * width) * 4 + 0], 
        //             pixels[(x + y * width) * 4 + 1], 
        //             pixels[(x + y * width) * 4 + 2], 
        //             pixels[(x + y * width) * 4 + 3]
        //         ); 
        //     }
        // }
    }

    void Texture2D::SetData(const void* data) 
    {
        SetData(0, 0, Width_, Height_, data); 
    }

    void Texture2D::SetFilter(TextureFilter filter) 
    {
        Filter_ = filter; 
        DirtyParams_ = true; 
    }

    void Texture2D::SetAddressMode(AddressMode mode) 
    {
        AddressMode_ = mode; 
        DirtyParams_ = true; 
    }

}