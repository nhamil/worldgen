#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{
    struct GraphicsAPI
    {
        GraphicsState Settings; 
        GPUHandle ShaderId = 0; 
        GPUHandle VertexArrayId = 0; 
        GPUHandle FramebufferReadId = 0; 
        GPUHandle FramebufferDrawId = 0; 
        unsigned ActiveTexture = 0; 
        unsigned TextureType[Graphics::MaxTextureCount]; 
        GPUHandle TextureIds[Graphics::MaxTextureCount]; 
        bool TextureIdsDirty = true; 
        GPUHandle DefaultVAO = 0; 

        GraphicsAPI(); 
        ~GraphicsAPI(); 

        void SetState(const GraphicsState& state); 
        void BindShader(GPUHandle id); 
        void BindVertexArray(GPUHandle id); 
        void BindVertexBuffer(GPUHandle id, bool vao); 
        void BindIndexbuffer(GPUHandle id, bool vao); 
        void BindTexture2D(GPUHandle id, int index = -1); 
        void BindFramebuffer(GPUHandle read, GPUHandle draw); 

        void DeleteShader(GPUHandle id); 
        void DeleteVertexArray(GPUHandle id); 
        void DeleteVertexBuffer(GPUHandle id); 
        void DeleteIndexBuffer(GPUHandle id); 
        void DeleteTexture2D(GPUHandle id); 
        void DeleteFramebuffer(GPUHandle id); 
    };
}