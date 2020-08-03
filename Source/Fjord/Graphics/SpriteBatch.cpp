#include "Fjord/Graphics/SpriteBatch.h" 

#include "Fjord/Core/Window.h" 
#include "Fjord/Graphics/Graphics.h"
#include "Fjord/Math/MathUtil.h" 

namespace Fjord 
{

    static const char* DefaultVertexSource = R"(#version 330 
        in vec3 a_Position; 
        in vec4 a_Color0; 
        in vec2 a_Texture0; 

        out vec4 v_Color; 
        out vec2 v_Texture; 

        uniform mat4 u_Matrix; 

        void main() 
        {
            v_Color = a_Color0; 
            v_Texture = a_Texture0; 
            gl_Position = u_Matrix * vec4(a_Position, 1.0); 
        }
    )"; 

    static const char* DefaultFragmentSource = R"(#version 330 
        in vec4 v_Color; 
        in vec2 v_Texture; 

        out vec4 f_Color; 

        uniform sampler2D u_Texture; 

        void main() 
        {
            f_Color = v_Color * texture(u_Texture, v_Texture); 
        }
    )"; 

    static const char* DefaultTextVertexSource = R"(#version 330 
        in vec3 a_Position; 
        in vec4 a_Color0; 
        in vec2 a_Texture0; 

        out vec4 v_Color; 
        out vec2 v_Texture; 

        uniform mat4 u_Matrix; 

        void main() 
        {
            v_Color = a_Color0; 
            v_Texture = a_Texture0; 
            gl_Position = u_Matrix * vec4(a_Position, 1.0); 
        }
    )"; 

    static const char* DefaultTextFragmentSource = R"(#version 330 
        in vec4 v_Color; 
        in vec2 v_Texture; 

        out vec4 f_Color; 

        uniform sampler2D u_Texture; 

        void main() 
        {
            f_Color = v_Color; 
            f_Color.a *= texture(u_Texture, v_Texture).r; 
        }
    )";

    Ref<Shader> SpriteBatch::DefaultShader_ = nullptr; 
    Ref<Shader> SpriteBatch::DefaultTextShader_ = nullptr; 
    Ref<Texture2D> SpriteBatch::WhiteTexture_ = nullptr; 
    Ref<Font> SpriteBatch::DefaultFont_ = nullptr; 

    SpriteBatch::SpriteBatch() 
    {
        CreateDefaultResourcesIfNeeded(); 

        Vertices_ = new VertexBuffer(VertexFormat::PositionTextureColor, MaxSpriteCount * 4); 
        Indices_ = new IndexBuffer(MaxSpriteCount * 6); 
        Geometry_ = new Geometry(); 

        Texture_ = WhiteTexture_; 
        Shader_ = DefaultShader_; 
        TextShader_ = DefaultTextShader_; 

        Vector<uint32> inds; 
        inds.reserve(MaxSpriteCount * 6); 
        for (unsigned i = 0; i < MaxSpriteCount; i++) 
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

    SpriteBatch::~SpriteBatch() 
    {
        // refs don't need to be released 
    } 

    void SpriteBatch::Begin() 
    {
        GetGraphics()->SetDepthTest(false); 
    }

    void SpriteBatch::End() 
    {
        Flush(); 
    }

    void SpriteBatch::SetTextShader(Shader* shader) 
    {
        Ref<Shader> newShader = shader ? shader : DefaultTextShader_.Get(); 

        if (newShader.Get() == TextShader_.Get())  
        {
            return; 
        }
        else 
        {
            Flush(); 
            TextShader_ = newShader; 
        }
    } 

    void SpriteBatch::SetDepth(float z) 
    {
        Z_ = z; 
    }

    void SpriteBatch::SetShader(Shader* shader) 
    {
        Ref<Shader> newShader = shader ? shader : DefaultShader_.Get(); 

        if (newShader.Get() == Shader_.Get())  
        {
            return; 
        }
        else 
        {
            Flush(); 
            Shader_ = newShader; 
        }
    } 

    void SpriteBatch::SetTexture(Texture2D* tex) 
    {
        Ref<Texture2D> newTex = tex ? tex : WhiteTexture_.Get(); 

        if (newTex.Get() == Texture_.Get())  
        {
            return; 
        }
        else 
        {
            Flush(); 
            Texture_ = newTex; 
        }
    } 

    void SpriteBatch::Draw(Color col, float x, float y, float w, float h) 
    {
        Draw(
            nullptr, col, 
            x, y, w, h, 
            0, 0, 0, 0
        ); 
    }

    void SpriteBatch::Draw(Texture2D* tex, float x, float y, float w, float h) 
    {
        float tw = 0, th = 0; 
        if (tex) 
        {
            tw = tex->GetWidth(); 
            th = tex->GetHeight(); 
        }
        Draw(
            tex, Color::White, 
            x, y, w, h, 
            0, 0, tw, th
        ); 
    }

    void SpriteBatch::Draw(Texture2D* tex, Color col, float x, float y, float w, float h) 
    {
        float tw = 0, th = 0; 
        if (tex) 
        {
            tw = tex->GetWidth(); 
            th = tex->GetHeight(); 
        }
        Draw(
            tex, col, 
            x, y, w, h, 
            0, 0, tw, th
        ); 
    }

    void SpriteBatch::Draw(
        Texture2D* tex, 
        float x, float y, float w, float h, 
        float tx, float ty, float tw, float th) 
    {
        Draw(
            tex, Color::White, 
            x, y, w, h, 
            tx, ty, tw, th
        ); 
    }

    void SpriteBatch::Draw(
        Texture2D* tex, Color col, 
            float x, float y, float w, float h, 
            float tx, float ty, float tw, float th)
    {
        SetTextMode(false); 
        SetTexture(tex); 
        
        float z = Z_; 

        float invW = Texture_->GetInvWidth(); 
        float invH = Texture_->GetInvHeight(); 
        float Tx0 = tx * invW; 
        float Tx1 = (tx + tw) * invW; 
        float Ty0 = ty * invH; 
        float Ty1 = (ty + th) * invH; 

        float x0 = x, y0 = y, x1 = x + w, y1 = y + h; 
        float fx0, fy0, fx1, fy1; 
        float tx0, ty0, tx1, ty1; 
        
        if (!Clip(x0, y0, x1, y1, fx0, fy0, fx1, fy1)) return; // rectangle fully out of bounds 

        tx0 = Lerp(Tx0, Tx1, fx0); 
        ty0 = Lerp(Ty0, Ty1, fy0); 
        tx1 = Lerp(Tx0, Tx1, fx1); 
        ty1 = Lerp(Ty0, Ty1, fy1); 

        VertexPositionTextureColor verts[4] = {
            {{x0, y0, z}, {tx0, ty0}, col}, 
            {{x1, y0, z}, {tx1, ty0}, col}, 
            {{x1, y1, z}, {tx1, ty1}, col}, 
            {{x0, y1, z}, {tx0, ty1}, col}, 
        };
        AddRect(verts); 
    }  

    void SpriteBatch::DrawString(Color col, const char* str, float x, float y) 
    {
        DrawString(nullptr, col, str, x, y); 
    } 

    void SpriteBatch::DrawString(Color col, float size, const char* str, float x, float y) 
    {
        DrawString(nullptr, col, size, str, x, y); 
    } 

    void SpriteBatch::DrawString(Font* f, Color col, const char* str, float x, float y) 
    {
        FontFace* font = f ? f->GetFace() : DefaultFont_.Get()->GetFace(); 
        DrawString(font, col, font->GetSize(), str, x, y); 
    }

    void SpriteBatch::DrawString(FontFace* font, Color col, float size, const char* str, float x, float y) 
    {
        if (!font) font = DefaultFont_.Get()->GetFace(); 

        FJ_EASSERT(font); 

        SetTextMode(true); 
        SetTextShader(nullptr); 
        SetTexture(font->GetTexture()); 

        float z = Z_; 
        float iW = font->GetTexture()->GetInvWidth(); 
        float iH = font->GetTexture()->GetInvHeight(); 

        float startX = x; 

        float scale = size / font->GetSize(); 

        unsigned len = strlen(str); 
        for (unsigned i = 0; i < len; i++) 
        {
            if (str[i] == '\n') 
            {
                y += size * 1.1; 
                x = startX; 
                continue; 
            }
            const Glyph* glyph = font->GetGlyph(str[i]); 

            if (!glyph) 
            {
                FJ_EFWARN("No glyph for '%c' (%d)", str[i], (int) str[i]); 
                continue; 
            }

            float x0 = x + (glyph->BearingX - 2) * scale; 
            float y0 = y - (glyph->BearingY + 2) * scale; 
            float x1 = x0 + (glyph->Width + 4) * scale; 
            float y1 = y0 + (glyph->Height + 4) * scale; 

            x += glyph->Advance * scale; 

            float Tx0 = (glyph->TextureX - 2) * iW; 
            float Ty0 = (glyph->TextureY - 2) * iH; 
            float Tx1 = (glyph->TextureX + glyph->Width + 2) * iW; 
            float Ty1 = (glyph->TextureY + glyph->Height + 2) * iH; 

            float fx0, fy0, fx1, fy1; 
            float tx0, ty0, tx1, ty1; 
            
            if (!Clip(x0, y0, x1, y1, fx0, fy0, fx1, fy1)) continue; 

            tx0 = Lerp(Tx0, Tx1, fx0); 
            ty0 = Lerp(Ty0, Ty1, fy0); 
            tx1 = Lerp(Tx0, Tx1, fx1); 
            ty1 = Lerp(Ty0, Ty1, fy1); 

            VertexPositionTextureColor verts[4] = {
                {{x0, y0, z}, {tx0, ty0}, col}, 
                {{x1, y0, z}, {tx1, ty0}, col}, 
                {{x1, y1, z}, {tx1, ty1}, col}, 
                {{x0, y1, z}, {tx0, ty1}, col}, 
            };
            AddRect(verts); 
        }
    }

    void SpriteBatch::Flush() 
    {
        if (CurRect_ == 0) return; 

        auto graphics = GetGraphics(); 

        graphics->SetDepthTest(false); 

        Vertices_->Update(); 

        Shader* shader = TextMode_ ? TextShader_ : Shader_; 

        shader->SetMatrix4("u_Matrix", Matrix4::Orthographic(
            {0, (float) graphics->GetHeight(), 1}, 
            {(float) graphics->GetWidth(), 0, -1}
        )); 
        shader->SetTextureUnit("u_Texture", 0); 
        shader->Update(); 

        graphics->SetShader(shader); 
        graphics->SetGeometry(Geometry_); 
        graphics->SetTexture(0, Texture_); 
        graphics->DrawIndexed(Primitive::Triangles, 0, CurRect_ * 6); 

        CurRect_ = 0; 
    }  

    void SpriteBatch::CreateDefaultResourcesIfNeeded() 
    {
        if (!DefaultShader_) 
        {
            DefaultShader_ = new Shader(DefaultVertexSource, DefaultFragmentSource); 
        }

        if (!DefaultTextShader_) 
        {
            DefaultTextShader_ = new Shader(DefaultTextVertexSource, DefaultTextFragmentSource); 
        }

        if (!DefaultFont_) 
        {
            DefaultFont_ = new Font("Default"); 
        }

        if (!WhiteTexture_) 
        {
            WhiteTexture_ = new Texture2D(1, 1, TextureFormat::RGBA8); 
            uint8 col[] = { 0xFF, 0xFF, 0xFF, 0xFF}; 
            WhiteTexture_->SetData(col); 
            WhiteTexture_->Update(); 
        }
    }  

    void SpriteBatch::SetTextMode(bool text) 
    {
        if (TextMode_ != text) 
        {
            Flush(); 
        }

        TextMode_ = text; 
    }

    void SpriteBatch::AddRect(VertexPositionTextureColor* rect) 
    {
        Vertices_->SetData(CurRect_ * 4, 4, rect); 
        CurRect_++; 

        if (CurRect_ == MaxSpriteCount) 
        {
            Flush(); 
        }
    }

    void SpriteBatch::SetBounds(float x, float y, float w, float h) 
    {
        Bounds_.Enabled = true; 
        Bounds_.X = x; 
        Bounds_.Y = y; 
        Bounds_.W = w; 
        Bounds_.H = h; 
    }

    void SpriteBatch::ClearBounds() 
    {
        Bounds_.Enabled = false; 
    }

    bool SpriteBatch::Clip(float& x1, float& y1, float& x2, float& y2, float& xMix1, float& yMix1, float& xMix2, float& yMix2) 
    {
        if (!Bounds_.Enabled) 
        {
            xMix1 = 0; 
            xMix2 = 1; 
            yMix1 = 0; 
            yMix2 = 1; 
            return true; 
        }

        float cx1 = Bounds_.X; 
        float cy1 = Bounds_.Y; 
        float cx2 = cx1 + Bounds_.W;  
        float cy2 = cy1 + Bounds_.H; 

        float x = x1; 
        float y = y1; 
        float iw = (x2 == x1) ? 0 : (1.0 / (x2 - x1)); 
        float ih = (y2 == y1) ? 0 : (1.0 / (y2 - y1)); 

        if (cx1 > x2) return false; 
        if (cx2 < x1) return false; 
        if (cy1 > y2) return false; 
        if (cy2 < y1) return false; 

        if (cx1 > x1) x1 = cx1; 
        if (cx2 < x2) x2 = cx2; 
        if (cy1 > y1) y1 = cy1; 
        if (cy2 < y2) y2 = cy2; 

        xMix1 = (x1 - x) * iw; 
        xMix2 = (x2 - x) * iw; 
        yMix1 = (y1 - y) * ih; 
        yMix2 = (y2 - y) * ih; 

        return true; 
    }

}