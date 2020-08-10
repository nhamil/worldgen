#include "Fjord/Graphics/GraphicsAPI.h" 

#include "Fjord/Graphics/OpenGL.h" 

namespace Fjord 
{

    GraphicsAPI::GraphicsAPI() 
    {
        GLCALL(glGenVertexArrays(1, &DefaultVAO)); 
        BindVertexArray(DefaultVAO); 
    }

    GraphicsAPI::~GraphicsAPI() 
    {

    }

    void GraphicsAPI::SetState(const GraphicsState& state) 
    {
        FJ_EFDEBUG("TODO SetState"); 
    }

    void GraphicsAPI::BindShader(GPUHandle id) 
    {
        if (ShaderId == id) return; 
        ShaderId = id; 
        GLCALL(glUseProgram(id)); 
    }

    void GraphicsAPI::DeleteShader(GPUHandle id) 
    {
        GLCALL(glDeleteProgram(id)); 
        if (ShaderId == id) 
        {
            BindShader(0); 
        }
    }

    void GraphicsAPI::BindVertexArray(GPUHandle id) 
    {
        id = id == 0 ? DefaultVAO : id; 
        if (VertexArrayId == id) return; 
        GLCALL(glBindVertexArray(id)); 
    }

    void GraphicsAPI::DeleteVertexArray(GPUHandle id) 
    {
        GLCALL(glDeleteVertexArrays(1, &id)); 
        if (VertexArrayId == id) 
        {
            BindVertexArray(0); 
        }
    }

    void GraphicsAPI::BindVertexBuffer(GPUHandle id, bool vao) 
    {
        if (!vao) BindVertexArray(0); 
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, id)); 
    }

    void GraphicsAPI::DeleteVertexBuffer(GPUHandle id) 
    {
        GLCALL(glDeleteBuffers(1, &id)); 
    }

    void GraphicsAPI::BindIndexbuffer(GPUHandle id, bool vao) 
    {
        if (!vao) BindVertexArray(0); 
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id)); 
    }

    void GraphicsAPI::DeleteIndexBuffer(GPUHandle id) 
    {
        GLCALL(glDeleteBuffers(1, &id)); 
    }

    void GraphicsAPI::BindTexture2D(GPUHandle id, int index) 
    {
        unsigned ind = index == -1 ? ActiveTexture : index; 
        if (TextureIds[ind] == id) return; 

        if (ind != ActiveTexture) GLCALL(glActiveTexture(GL_TEXTURE0 + ind)); 
        GLCALL(glBindTexture(GL_TEXTURE_2D, id)); 

        ActiveTexture = ind; 
        TextureIds[ind] = id; 
    }

    void GraphicsAPI::DeleteTexture2D(GPUHandle id) 
    {
        GLCALL(glDeleteTextures(1, &id)); 
        for (unsigned i = 0; i < Graphics::MaxTextureCount; i++) 
        {
            if (TextureIds[i] == id) 
            {
                BindTexture2D(id, 0); 
            }
        }
    }

}