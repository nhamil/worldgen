#pragma once 

#include "Fjord/Graphics/GraphicsAPI.h" 

#include <GL/glew.h> 

#include <iostream> 

namespace Fjord 
{
    const char* GetOpenGLErrorString(GLenum err); 

    inline GLenum GetGLAddressMode(AddressMode mode) 
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

    inline GLenum GetGLMinFilter(TextureFilter filter) 
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

    inline GLenum GetGLMagFilter(TextureFilter filter) 
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

    inline GLenum GetGLTextureInternalFormat(TextureFormat format) 
    {
        switch (format) 
        {
            case TextureFormat::Depth16: return GL_DEPTH_COMPONENT16; 
            case TextureFormat::Depth24: return GL_DEPTH_COMPONENT24; 
            case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8; 
            case TextureFormat::Depth32: return GL_DEPTH_COMPONENT32; 
            case TextureFormat::RGBA16F: return GL_RGBA16F; 
            case TextureFormat::RGBA32F: return GL_RGBA32F; 
            case TextureFormat::RGBA8: return GL_RGBA8; 
            case TextureFormat::RGB8: return GL_RGB8; 
            case TextureFormat::RG8: return GL_RG8; 
            case TextureFormat::R8: return GL_R8; 
            default: 
                FJ_EFWARN("Unsupported internal texture format: %d", format); 
                return GL_R8; 
        }
    }

    inline GLenum GetGLTextureInputFormat(TextureFormat format) 
    {
        if (IsTextureFormatDepthFormat(format)) return GL_DEPTH_COMPONENT; 
        switch (format) 
        {
            case TextureFormat::Depth16: return GL_DEPTH_COMPONENT; 
            case TextureFormat::Depth24: return GL_DEPTH_COMPONENT; 
            case TextureFormat::Depth24Stencil8: return GL_DEPTH_STENCIL; 
            case TextureFormat::Depth32: return GL_DEPTH_COMPONENT; 
            case TextureFormat::RGBA16F: return GL_RGBA; 
            case TextureFormat::RGBA32F: return GL_RGBA; 
            case TextureFormat::RGBA8: return GL_RGBA; 
            case TextureFormat::RGB8: return GL_RGB; 
            case TextureFormat::RG8: return GL_RG; 
            case TextureFormat::R8: return GL_RED; 
            default: 
                FJ_EFWARN("Unsupported input texture format: %d", format); 
                return GL_R; 
        }
    }

    inline GLenum GetGLTextureInputType(TextureFormat format) 
    {
        if (IsTextureFormatDepthFormat(format)) return GL_UNSIGNED_INT; 
        switch (format) 
        {
            case TextureFormat::Depth16: return GL_UNSIGNED_INT; 
            case TextureFormat::Depth24: return GL_UNSIGNED_INT; 
            case TextureFormat::Depth24Stencil8: return GL_UNSIGNED_INT; 
            case TextureFormat::Depth32: return GL_UNSIGNED_INT; 
            case TextureFormat::RGBA16F: return GL_FLOAT; 
            case TextureFormat::RGBA32F: return GL_FLOAT; 
            case TextureFormat::RGBA8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::RGB8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::RG8: return GL_UNSIGNED_BYTE; 
            case TextureFormat::R8: return GL_UNSIGNED_BYTE; 
            default: 
                FJ_EFWARN("Unsupported input texture type: %d", format); 
                return GL_UNSIGNED_BYTE; 
        }
    }
}

#define GLCALL(x) { \
    x; \
    int glerror = glGetError(); \
    if (glerror != GL_NO_ERROR) FJ_EFLOG(Error, "OpenGL Error (%d): %s", glerror, GetOpenGLErrorString(glerror)); }
