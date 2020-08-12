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

    inline GLenum GetGLBlendMode(BlendMode mode) 
    {
        switch (mode) 
        {
            case BlendMode::One: return GL_ONE; 
            case BlendMode::Zero: return GL_ZERO; 
            case BlendMode::SourceAlpha: return GL_SRC_ALPHA; 
            case BlendMode::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA; 
            default: 
                FJ_EFLOG(Warn, "Unknown blend mode: %d", mode); 
                return 0; 
        }
    }

    GraphicsAPI* GPUObject::GetAPI() 
    {
        return GetGraphics()->GetAPI(); 
    }

    Graphics::Graphics() 
    {
        SetDepthTest(true); 
        GLCALL(glEnable(GL_BLEND));
        // GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); 
        SetBlendMode(BlendMode::SourceAlpha, BlendMode::OneMinusSourceAlpha); 
        GLCALL(glEnable(GL_MULTISAMPLE)); 
        // GLCALL(glEnable(GL_SCISSOR_TEST)); 

        API_ = new GraphicsAPI(); 
    }

    Graphics::~Graphics() 
    {
        delete API_; 
    }

    void Graphics::BeginFrame() 
    {
        SetShader(nullptr); 
        SetGeometry(nullptr); 
        SetRenderTarget(nullptr); 
        ClearTextures(); 
        SetClearColor(Color::Black); 
        Clear(true, true); 
        ResetViewport(); 
        // ResetClip(); 
    }

    void Graphics::EndFrame() 
    {

    }

    unsigned Graphics::GetWidth() 
    {
        return CurRenderTarget_ ? CurRenderTarget_->GetWidth() : (unsigned) GetWindow()->GetWidth(); 
    }

    unsigned Graphics::GetHeight() 
    {
        return CurRenderTarget_ ? CurRenderTarget_->GetHeight() : (unsigned) GetWindow()->GetHeight(); 
    }

    unsigned Graphics::GetScreenWidth() 
    {
        return GetWindow()->GetWidth(); 
    }

    unsigned Graphics::GetScreenHeight() 
    {
        return GetWindow()->GetHeight(); 
    }

    float Graphics::GetAspectRatio() 
    {
        return (float) GetWidth() / GetHeight(); 
    }

    void Graphics::SetViewport(int x, int y, int w, int h) 
    {
        GLCALL(glViewport(x, y, w, h)); 
    }

    void Graphics::ResetViewport() 
    {
        GLCALL(glViewport(0, 0, GetWidth(), GetHeight())); 
    }

    // void Graphics::SetClip(int x, int y, int w, int h) 
    // {
    //     auto* window = GetWindow(); 
    //     // GLCALL(glEnable(GL_SCISSOR_TEST)); 

    //     // GLCALL(glScissor(x, window->GetHeight() - y - h, w, h)); 
    // }

    // void Graphics::ResetClip() 
    // {
    //     auto* window = GetWindow(); 
    //     // GLCALL(glDisable(GL_SCISSOR_TEST)); 
    //     // GLCALL(glScissor(0, 0, window->GetWidth(), window->GetHeight())); 
    // }

    void Graphics::ClearTextures() 
    {
        for (unsigned i = 0; i < MaxTextureCount; i++) 
        {
            CurTextures_[i] = nullptr; 
        }
    }

    void Graphics::SetTexture(unsigned index, Texture* tex) 
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

    void Graphics::SetRenderTarget(RenderTarget* target) 
    {
        if (target && !target->IsValid()) 
        {
            FJ_EFWARN("Attempted to set invalid render target"); 
            target = nullptr; 
        }
        CurRenderTarget_ = target; 
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

    void Graphics::SetBlendMode(BlendMode src, BlendMode dst) 
    {
        GLCALL(glBlendFunc(GetGLBlendMode(src), GetGLBlendMode(dst))); 
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
        PrepareRenderTarget(); 

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
        FJ_EASSERT(CurShader_ && CurGeom_ && CurGeom_->HasUsableVertexBuffer()); 

        PrepareDraw(); 

        GLCALL(glDrawArrays(
            GetGLPrimitive(prim), 
            startInd, 
            numInds 
        )); 
    }

    void Graphics::DrawIndexed(Primitive prim, unsigned startInd, unsigned numInds) 
    {
        FJ_EASSERT(CurShader_ && CurGeom_ && CurGeom_->HasUsableVertexBuffer() && CurGeom_->GetIndexBuffer()); 

        PrepareDraw(); 

        GLCALL(glDrawElements(
            GetGLPrimitive(prim), 
            numInds, 
            GL_UNSIGNED_INT, 
            (void*) (startInd * sizeof(uint32)) 
        )); 
    }

    void Graphics::PrepareRenderTarget() 
    {
        GPUHandle fbo = 0; 
        if (CurRenderTarget_) 
        {
            CurRenderTarget_->Update(); 
            fbo = CurRenderTarget_->GetHandle(); 
        }
        API_->BindFramebuffer(fbo, fbo); 
    }

    void Graphics::PrepareDraw() 
    {
        PrepareRenderTarget(); 

        CurShader_->Update(); 
        CurGeom_->Update(); 
        
        for (unsigned i = 0; i < MaxTextureCount; i++) 
        {
            // GLCALL(glActiveTexture(GL_TEXTURE0 + i)); 
            if (CurTextures_[i]) 
            {
                // GLCALL(glBindTexture(GL_TEXTURE_2D, CurTextures_[i]->GetHandle())); 
                API_->BindTexture2D(CurTextures_[i]->GetHandle(), (int) i); 
            }
            else 
            {
                // GLCALL(glBindTexture(GL_TEXTURE_2D, 0)); 
                API_->BindTexture2D(0, (int) i); 
            }
        }

        API_->BindShader(CurShader_->GetHandle()); 
        API_->BindVertexArray(CurGeom_->GetHandle()); 
    }

}