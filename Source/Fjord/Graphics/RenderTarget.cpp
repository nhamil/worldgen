#include "Fjord/Graphics/RenderTarget.h" 

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Math/MathUtil.h" 

namespace Fjord 
{

    RenderTarget::RenderTarget() 
    {
        GLCALL(glGenFramebuffers(1, &Handle_)); 
    }

    RenderTarget::RenderTarget(unsigned width, unsigned height, TextureFormat depthFormat, TextureFormat colorFormat) 
    {
        GLCALL(glGenFramebuffers(1, &Handle_)); 

        SetDepthBuffer(new RenderTexture(width, height, depthFormat)); 
        SetColorBufferCount(1); 
        SetColorBuffer(0, new RenderTexture(width, height, colorFormat)); 
        Update(); 
    }

    RenderTarget::~RenderTarget() 
    {
        if (Handle_) 
        {
            // GLCALL(glDeleteFramebuffers(1, &Handle_)); 
            GetAPI()->DeleteFramebuffer(Handle_); 
            Handle_ = 0; 
        }
    }

    void RenderTarget::Update() 
    {
        if (DepthBuffer_) DepthBuffer_->Update(); 
        for (Ref<RenderTexture>& tex : ColorBuffers_) 
        {
            if (tex) tex->Update(); 
        }

        if (Dirty_) 
        {
            // GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, Handle_)); 
            GetAPI()->BindFramebuffer(Handle_, Handle_); 

            if (DepthBuffer_) 
            {
                GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthBuffer_->GetHandle(), 0));
            }
            else 
            {
                GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0));
            }

            Vector<GLuint> attachments; 
            for (unsigned i = 0; i < MaxColorBufferCount; i++) 
            {
                if (i < GetColorBufferCount() && ColorBuffers_[i]) 
                {
                    GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, ColorBuffers_[i]->GetHandle(), 0));
                    attachments.push_back(GL_COLOR_ATTACHMENT0 + i); 
                }
                else 
                {
                    GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0));
                }
            }
            GLCALL(glDrawBuffers(attachments.size(), &attachments[0])); 

            // GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); 
            GetAPI()->BindFramebuffer(0, 0); 
            Dirty_ = false; 
        }
    }

    unsigned RenderTarget::GetWidth() const 
    {
        if (!IsValid()) return 0; 

        unsigned width = UINT32_MAX; 
        if (HasDepthBuffer()) width = DepthBuffer_->GetWidth(); 

        for (const Ref<RenderTexture>& tex : ColorBuffers_) 
        {
            unsigned w = tex->GetWidth(); 
            if (w < width) w = width; 
        }

        return width; 
    }

    unsigned RenderTarget::GetHeight() const 
    {
        if (!IsValid()) return 0; 

        unsigned height = UINT32_MAX; 
        if (HasDepthBuffer()) height = DepthBuffer_->GetHeight(); 

        for (const Ref<RenderTexture>& tex : ColorBuffers_) 
        {
            unsigned h = tex->GetHeight(); 
            if (h < height) h = height; 
        }

        return height; 
    }

    unsigned RenderTarget::GetColorBufferCount() const 
    {
        return ColorBuffers_.size(); 
    }

    bool RenderTarget::HasDepthBuffer() const 
    {
        return (bool) DepthBuffer_; 
    }

    bool RenderTarget::IsValid() const 
    {
        // GLenum complete; 
        // GLCALL(complete = glCheckFramebufferStatus(Handle_)); 
        // if (complete != GL_FRAMEBUFFER_COMPLETE) return false; 

        if (DepthBuffer_ != nullptr) return true; 

        for (unsigned i = 0; i < GetColorBufferCount(); i++) 
        {
            if (ColorBuffers_[i]) return true; 
        }

        return false; 
    }

    RenderTexture* RenderTarget::GetDepthBuffer() const 
    {
        return DepthBuffer_; 
    }

    RenderTexture* RenderTarget::GetColorBuffer(unsigned index) const 
    {
        FJ_EASSERT(index < GetColorBufferCount()); 
        return ColorBuffers_[index]; 
    }

    void RenderTarget::Clear() 
    {
        SetDepthBuffer(nullptr); 
        SetColorBufferCount(0); 
    }

    void RenderTarget::SetDepthBuffer(RenderTexture* tex) 
    {
        if (tex == DepthBuffer_) return; 

        Dirty_ = true; 
        if (tex) 
        {   
            FJ_EASSERT(IsTextureFormatDepthFormat(tex->GetFormat())); 
            DepthBuffer_ = tex; 
        }
        else 
        {
            DepthBuffer_ = nullptr; 
        }
    }

    void RenderTarget::SetColorBufferCount(unsigned count) 
    {
        FJ_EASSERT(count < MaxColorBufferCount); 
        ColorBuffers_.resize(count); 
    }

    void RenderTarget::SetColorBuffer(unsigned index, RenderTexture* tex) 
    {
        FJ_EASSERT(index < MaxColorBufferCount); 
        if (ColorBuffers_[index] == tex) return; 

        Dirty_ = true; 
        if (tex) 
        {   
            FJ_EASSERT(IsTextureFormatColorFormat(tex->GetFormat())); 
            ColorBuffers_[index] = tex; 
        }
        else 
        {
            ColorBuffers_[index] = nullptr; 
        }
    }

}