#include "Fjord/Graphics/Graphics.h" 

#include "Fjord/Core/Window.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/IndexBuffer.h" 
#include "Fjord/Graphics/Geometry.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Graphics/Texture2D.h" 
#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Graphics/VertexBuffer.h" 

namespace Fjord 
{

    inline GLuint GetGLPrimitive(Primitive prim) 
    {
        switch (prim) 
        {
            case Primitive::Points: return GL_POINTS; 
            case Primitive::Lines: return GL_LINES; 
            case Primitive::Triangles: return GL_TRIANGLES; 
            default: 
                FJ_EFLOG(Warn, "Unknown primitive type: %d", prim); 
                return 0; 
        }
    }

    Graphics::Graphics() 
    {
        SetDepthTest(true); 
        GLCALL(glEnable(GL_BLEND));
        GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); 
        GLCALL(glEnable(GL_SCISSOR_TEST)); 
    }

    Graphics::~Graphics() 
    {
        
    }

    void Graphics::SetViewport(int x, int y, int w, int h) 
    {
        auto* window = GetWindow(); 

        FJ_EFWARN("SetViewport: (0,0) should be top left"); 
        GLCALL(glViewport(x, y, w, h)); 
    }

    void Graphics::ResetViewport() 
    {
        auto* w = GetWindow(); 
        GLCALL(glViewport(0, 0, w->GetWidth(), w->GetHeight())); 
    }

    void Graphics::SetClip(int x, int y, int w, int h) 
    {
        auto* window = GetWindow(); 
        // GLCALL(glEnable(GL_SCISSOR_TEST)); 

        GLCALL(glScissor(x, window->GetHeight() - y - h, w, h)); 
    }

    void Graphics::ResetClip() 
    {
        auto* window = GetWindow(); 
        // GLCALL(glDisable(GL_SCISSOR_TEST)); 
        GLCALL(glScissor(0, 0, window->GetWidth(), window->GetHeight())); 
    }

    void Graphics::ClearTextures() 
    {
        for (unsigned i = 0; i < MaxTextureCount; i++) 
        {
            CurTextures_[i] = nullptr; 
        }
    }

    void Graphics::SetTexture(unsigned index, Texture2D* tex) 
    {
        FJ_EASSERT(index < MaxTextureCount); 

        CurTextures_[index] = tex; 
    }

    void Graphics::SetShader(Shader* shader) 
    {
        CurShader_ = shader; 
    }

    void Graphics::SetGeometry(Geometry* geom) 
    {
        CurGeom_ = geom; 
    }

    void Graphics::SetDepthTest(bool enabled) 
    {
        if (enabled) 
        {
            GLCALL(glEnable(GL_DEPTH_TEST)); 
            GLCALL(glDepthFunc(GL_LEQUAL)); 
        }
        else 
        {
            GLCALL(glDisable(GL_DEPTH_TEST)); 
        }
    }

    void Graphics::SetPointSize(float size) 
    {
        GLCALL(glPointSize(size)); 
    }

    void Graphics::SetLineWidth(float width) 
    {
        GLCALL(glLineWidth(width)); 
    }

    void Graphics::SetClearColor(const Color& color) 
    {
        GLCALL(glClearColor(
            color.R, 
            color.G, 
            color.B, 
            color.A 
        )); 
    }

    void Graphics::SetClearDepth(float depth) 
    {
        GLCALL(glClearDepth(depth)); 
    }

    void Graphics::Clear(bool color, bool depth) 
    {
        GLuint bits = 0; 
        if (color) bits |= GL_COLOR_BUFFER_BIT; 
        if (depth) bits |= GL_DEPTH_BUFFER_BIT; 
        if (bits) 
        {
            GLCALL(glClear(bits)); 
        }
    }

    void Graphics::Draw(Primitive prim, unsigned startInd, unsigned numInds) 
    {
        FJ_ASSERT(CurShader_ && CurGeom_ && CurGeom_->HasUsableVertexBuffer()); 

        CurShader_->Update(); 
        CurGeom_->Update(); 

        GLCALL(glUseProgram(CurShader_->GetHandle())); 
        GLCALL(glBindVertexArray(CurGeom_->GetHandle())); 

        GLCALL(glDrawArrays(
            GetGLPrimitive(prim), 
            startInd, 
            numInds 
        )); 
    }

    void Graphics::DrawIndexed(Primitive prim, unsigned startInd, unsigned numInds) 
    {
        FJ_ASSERT(CurShader_ && CurGeom_ && CurGeom_->HasUsableVertexBuffer() && CurGeom_->GetIndexBuffer()); 

        CurShader_->Update(); 
        CurGeom_->Update(); 

        // not sure that vertex arrays keep track of index buffers 
        // TODO figure this out 
        IndexBuffer* ib = CurGeom_->GetIndexBuffer(); 

        for (unsigned i = 0; i < MaxTextureCount; i++) 
        {
            GLCALL(glActiveTexture(GL_TEXTURE0 + i)); 
            if (CurTextures_[i]) 
            {
                GLCALL(glBindTexture(GL_TEXTURE_2D, CurTextures_[i]->GetHandle())); 
            }
            else 
            {
                GLCALL(glBindTexture(GL_TEXTURE_2D, 0)); 
            }
        }

        GLCALL(glUseProgram(CurShader_->GetHandle())); 
        GLCALL(glBindVertexArray(CurGeom_->GetHandle())); 
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->GetHandle())); 

        GLCALL(glDrawElements(
            GetGLPrimitive(prim), 
            numInds, 
            GL_UNSIGNED_INT, 
            (void*) (startInd * sizeof(uint32)) 
        )); 
    }

}