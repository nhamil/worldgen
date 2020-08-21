#include "Fjord/Graphics/RenderTargetSwap.h" 

#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/Graphics/VertexBuffer.h" 

namespace Fjord 
{

    RenderTargetSwap::RenderTargetSwap() 
    {
        SetSize(GetGraphics()->GetScreenWidth(), GetGraphics()->GetScreenHeight()); 

        VertexPositionTexture verts[] = {
            {{-1, -1, 0}, {0, 0}}, 
            {{ 1, -1, 0}, {1, 0}}, 
            {{ 1,  1, 0}, {1, 1}}, 
            {{-1, -1, 0}, {0, 0}}, 
            {{ 1,  1, 0}, {1, 1}}, 
            {{-1,  1, 0}, {0, 1}} 
        };
        VertexBuffer* vb = new VertexBuffer(VertexFormat::PositionTexture, 6); 
        vb->SetData(0, 6, verts); 
        vb->Update(); 
        Quad_ = new Geometry(); 
        Quad_->SetVertexBufferCount(1); 
        Quad_->SetVertexBuffer(0, vb); 
        Quad_->Update(); 

        QuadShader_ = Shader::Load("Quad"); 
        QuadShader_->SetTextureUnit("u_Texture", 0); 
        QuadShader_->Update(); 
    }

    RenderTargetSwap::~RenderTargetSwap() {}

    void RenderTargetSwap::Clear() 
    {
        auto* graphics = GetGraphics(); 
        graphics->SetRenderTarget(Dest_); 
        graphics->Clear(true, true); 
        graphics->SetRenderTarget(Source_); 
        graphics->Clear(true, true); 
    }

    void RenderTargetSwap::SetSize(unsigned w, unsigned h) 
    {
        if (Width_ != w || Height_ != h) 
        {
            Width_ = w; 
            Height_ = h; 
            Source_ = new RenderTarget(w, h, TextureFormat::Depth32, TextureFormat::RGBA16F); 
            Dest_ = new RenderTarget(w, h, TextureFormat::Depth32, TextureFormat::RGBA16F); 
        }
    }

    unsigned RenderTargetSwap::GetWidth() 
    {
        return Width_; 
    }

    unsigned RenderTargetSwap::GetHeight() 
    {
        return Height_; 
    }

    void RenderTargetSwap::SetSize(RenderTargetSwap* from) 
    {
        SetSize(from->GetWidth(), from->GetHeight()); 
    }

    void RenderTargetSwap::Swap() 
    {
        Ref<RenderTarget> tmp = Source_; 
        Source_ = Dest_; 
        Dest_ = tmp; 
    }

    RenderTarget* RenderTargetSwap::GetSource() 
    {
        return Source_; 
    }

    RenderTarget* RenderTargetSwap::GetDest() 
    {
        return Dest_; 
    }

    void RenderTargetSwap::ApplyEffect(RenderTarget* source, RenderTarget* dest, Shader* effect) 
    {
        auto* graphics = GetGraphics(); 
        graphics->SetRenderTarget(dest); 
        graphics->ResetViewport(); 
        graphics->SetBlendMode(BlendMode::One, BlendMode::Zero); 
        // graphics->SetBlendMode(BlendMode::SourceAlpha, BlendMode::OneMinusSourceAlpha); 
        graphics->SetGeometry(Quad_); 
        if (effect) 
        {
            graphics->SetShader(effect); 
        }
        else 
        {
            QuadShader_->SetTextureUnit("u_Texture", 0); 
            graphics->SetTexture(0, source->GetColorBuffer(0)); 
            graphics->SetShader(QuadShader_); 
        }
        graphics->Draw(Primitive::Triangles, 0, 6); 

        graphics->SetRenderTarget(nullptr); 
        graphics->ResetViewport(); 
    }

}