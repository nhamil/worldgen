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

    Ref<Shader> GUIRenderer::DefaultTextShader_ = nullptr; 
    Ref<Font> GUIRenderer::DefaultFont_ = nullptr; 

    void GUIRenderer::CreateDefaultResourcesIfNeeded() 
    {
        if (!DefaultTextShader_) 
        {
            DefaultTextShader_ = Shader::Load("../Assets/Shaders/Text"); 
        }

        if (!DefaultFont_) 
        {
            DefaultFont_ = new Font("../Assets/Fonts/Default.ttf"); 
        }
    }

    GUIRenderer::GUIRenderer() 
        : CurColor_(Color::Black) 
    {
        CreateDefaultResourcesIfNeeded(); 

        SpriteBatch_ = new SpriteBatch(); 
    }

    GUIRenderer::~GUIRenderer() 
    {
        // everything is ref counted so do nothing 
    }

    void GUIRenderer::SetFont(Font* font) 
    {
        CurFont_ = font; 
    }

    void GUIRenderer::SetColor(Color color) 
    {
        CurColor_ = color; 
    }

    void GUIRenderer::FillRect(float x, float y, float w, float h)  
    {
        SpriteBatch_->Draw(CurColor_, x, y, w, h); 
    }

    void GUIRenderer::Flush() 
    {
        SpriteBatch_->Flush(); 
    }

    Font* GUIRenderer::GetRenderFont() 
    {
        return CurFont_ ? CurFont_ : DefaultFont_; 
    }

}