#include "Fjord/Graphics/RenderTexture.h"

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/FileUtil.h" 
#include "Fjord/Util/Memory.h" 

namespace Fjord 
{

    RenderTexture::RenderTexture(unsigned width, unsigned height, TextureFormat format) 
        : Width_(width) 
        , Height_(height) 
        , InvWidth_(1.0 / width) 
        , InvHeight_(1.0 / height) 
        , Format_(format) 
    {
        GLCALL(glGenTextures(1, &Handle_)); 
    }

    RenderTexture::~RenderTexture() 
    {
        if (Handle_ != 0) 
        {
            // GLCALL(glDeleteTextures(1, &Handle_)); 
            GetAPI()->DeleteTexture2D(Handle_); 
            Handle_ = 0; 
        }
    }

    void RenderTexture::Update() 
    {
        if (!Dirty_ && !DirtyParams_) return; 

        // GLCALL(glBindTexture(GL_TEXTURE_2D, Handle_)); 
        GetAPI()->BindTexture2D(Handle_); 

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
                0
            )); 
        }

        // TODO do parameters affect mipmaps? 
        // GLCALL(glGenerateMipmap(GL_TEXTURE_2D)); 

        // GLCALL(glBindTexture(GL_TEXTURE_2D, 0)); 
        GetAPI()->BindTexture2D(0); 

        Dirty_ = DirtyParams_ = false; 
    }

    void RenderTexture::SetFilter(TextureFilter filter) 
    {
        Filter_ = filter; 
        DirtyParams_ = true; 
    }

    void RenderTexture::SetAddressMode(AddressMode mode) 
    {
        AddressMode_ = mode; 
        DirtyParams_ = true; 
    }

}