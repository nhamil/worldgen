#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Geometry.h" 
#include "Fjord/Graphics/RenderTarget.h" 
#include "Fjord/Graphics/Shader.h" 

namespace Fjord 
{

    class RenderTargetSwap : public RefCounted 
    {
    public: 
        RenderTargetSwap(); 
        ~RenderTargetSwap(); 

        unsigned GetWidth(); 
        unsigned GetHeight(); 

        void SetSize(unsigned w, unsigned h); 
        void SetSize(RenderTargetSwap* from); 
        void Swap(); 
        void Clear(); 

        RenderTarget* GetSource(); 
        RenderTarget* GetDest(); 

        /** 
         * `source` must be non-null. 
         * `dest` may be null to render to screen. 
         * `effect` may be null for default effect (blitting). 
         */ 
        void ApplyEffect(RenderTarget* source, RenderTarget* dest, Shader* effect); 

    private: 
        unsigned Width_, Height_; 
        Ref<RenderTarget> Source_, Dest_; 
        Ref<Geometry> Quad_; 
        Ref<Shader> QuadShader_; 
    };

}