#include "Fjord/GUI/GUIRenderer.h" 

#include "Fjord/Core/Window.h" 
#include "Fjord/Graphics/Graphics.h"
#include "Fjord/GUI/GUIElement.h" 

namespace Fjord 
{

    static const char* DefaultVertexSource = R"(#version 330 
        in vec3 a_Position; 
        in vec4 a_Color0; 

        out vec4 v_Color; 

        uniform mat4 u_Matrix; 

        void main() 
        {
            v_Color = a_Color0; 
            gl_Position = u_Matrix * vec4(a_Position, 1.0); 
        }
    )"; 

    static const char* DefaultFragmentSource = R"(#version 330 
        in vec4 v_Color; 

        out vec4 f_Color; 

        void main() 
        {
            f_Color = v_Color; 
        }
    )"; 

    Ref<Shader> GUIRenderer::DefaultShader_ = nullptr; 
    Ref<Font> GUIRenderer::DefaultFont_ = nullptr; 
    Ref<Texture2D> GUIRenderer::WhiteTexture_ = nullptr; 

    void GUIRenderer::CreateDefaultResourcesIfNeeded() 
    {
        if (!DefaultShader_) 
        {
            DefaultShader_ = new Shader(DefaultVertexSource, DefaultFragmentSource); 
        }

        if (!DefaultFont_) 
        {
            DefaultFont_ = new Font("../Assets/Fonts/Default.ttf"); 
        }

        if (!WhiteTexture_) 
        {
            WhiteTexture_ = new Texture2D(1, 1, TextureFormat::R8); 
            uint8 col = 0xFF; 
            WhiteTexture_->SetData(&col); 
            WhiteTexture_->Update(); 
        }
    }

    GUIRenderer::GUIRenderer() 
        : CurColor_(Color::Black) 
        , CurZOrder_(GUIElement::DefaultZOrder)
        , CurRectangle_(0) 
    {
        CreateDefaultResourcesIfNeeded(); 
        Shader_ = DefaultShader_; 

        Vertices_ = new VertexBuffer(VertexFormat::PositionColor, MaxRectangleCount * 4); 
        Indices_ = new IndexBuffer(MaxRectangleCount * 6); 
        Geometry_ = new Geometry(); 

        Vector<uint32> inds; 
        inds.reserve(MaxRectangleCount * 6); 
        for (unsigned i = 0; i < MaxRectangleCount; i++) 
        {
            inds.push_back(i*4 + 0); 
            inds.push_back(i*4 + 1); 
            inds.push_back(i*4 + 2); 
            inds.push_back(i*4 + 0); 
            inds.push_back(i*4 + 2); 
            inds.push_back(i*4 + 3); 
        }
        Indices_->SetData(inds); 

        Geometry_->SetIndexBuffer(Indices_); 
        Geometry_->SetVertexBufferCount(1); 
        Geometry_->SetVertexBuffer(0, Vertices_); 
        Geometry_->Update(); 
    }

    GUIRenderer::~GUIRenderer() 
    {
        // everything is ref counted so do nothing 
    }

    void GUIRenderer::SetFont(Font* font) 
    {
        CurFont_ = font; 
    }

    void GUIRenderer::SetFontSize(float px) 
    {
        CurFontSize_ = px; 
    } 

    void GUIRenderer::SetColor(const Color& color) 
    {
        CurColor_ = color; 
    }

    void GUIRenderer::SetZOrder(int z) 
    {
        CurZOrder_ = z; 
    }

    void GUIRenderer::FillRect(float x, float y, float w, float h)  
    {
        FlushIfNeeded(); 

        unsigned index = CurRectangle_ * 4; 

        float z = 0.0f; 

        VertexPositionColor verts[4] = {
            {{x, y, z}, CurColor_}, 
            {{x+w, y, z}, CurColor_}, 
            {{x+w, y+h, z}, CurColor_}, 
            {{x, y+h, z}, CurColor_}, 
        };

        Vertices_->SetData(index, 4, verts); 

        CurRectangle_++; 
    }

    void GUIRenderer::FlushIfNeeded() 
    {
        if (CurRectangle_ == MaxRectangleCount) 
        {
            Flush(); 
        }
    }

    void GUIRenderer::Flush() 
    {
        auto graphics = GetGraphics(); 
        auto window = GetWindow(); 

        graphics->SetDepthTest(true); 

        Vertices_->Update(); 

        Shader_->SetMatrix4("u_Matrix", Matrix4::Orthographic(
            {0, (float) window->GetHeight(), 1000}, 
            {(float) window->GetWidth(), 0, -1000}
        )); 

        graphics->SetShader(Shader_); 
        graphics->SetGeometry(Geometry_); 
        graphics->DrawIndexed(Primitive::Triangles, 0, CurRectangle_ * 6); 

        CurRectangle_ = 0; 
    }

    Font* GUIRenderer::GetRenderFont() 
    {
        return CurFont_ ? CurFont_ : DefaultFont_; 
    }

}