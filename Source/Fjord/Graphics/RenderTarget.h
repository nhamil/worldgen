#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/RenderTexture.h"
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class RenderTarget : public GPUObject 
    {
    public: 
        static const unsigned MaxColorBufferCount = 4; 

        RenderTarget(); 
        RenderTarget(unsigned width, unsigned height, TextureFormat depthFormat, TextureFormat colorFormat); 
        ~RenderTarget(); 

        void Update(); 

        unsigned GetColorBufferCount() const; 
        bool HasDepthBuffer() const; 

        bool IsValid() const; 

        unsigned GetWidth() const; 
        unsigned GetHeight() const; 

        RenderTexture* GetDepthBuffer() const; 
        RenderTexture* GetColorBuffer(unsigned index) const; 

        void Clear(); 
        void SetDepthBuffer(RenderTexture* tex); 
        void SetColorBufferCount(unsigned count); 
        void SetColorBuffer(unsigned index, RenderTexture* tex); 

        void SetColorBuffers(Vector<RenderTexture*> buffers) 
        {
            SetColorBufferCount(buffers.size()); 
            for (unsigned i = 0; i < buffers.size(); i++) 
            {
                SetColorBuffer(i, buffers[i]); 
            }
        }

    private: 
        Ref<RenderTexture> DepthBuffer_; 
        Vector<Ref<RenderTexture>> ColorBuffers_; 
        bool Dirty_ = true; 
    };

}