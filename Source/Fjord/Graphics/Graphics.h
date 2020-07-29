#pragma once 

#include "Fjord/Graphics/Types.h" 

namespace Fjord 
{

    class Graphics
    {
    public: 
        static const unsigned MaxTextureCount = 16;

        Graphics(); 
        ~Graphics(); 

        void ClearTextures(); 
        void SetTexture(unsigned index, Texture2D* texture); 
        void SetShader(Shader* shader); 
        void SetGeometry(Geometry* geom); 

        void SetDepthTest(bool enabled); 

        void SetPointSize(float size); 
        void SetLineWidth(float width); 

        void SetClearColor(const Color& color); 
        void SetClearDepth(float depth); 
        void Clear(bool color, bool depth); 

        void Draw(Primitive prim, unsigned startInd, unsigned numInds); 
        void DrawIndexed(Primitive prim, unsigned startInd, unsigned numInds); 

    private: 
        Ref<Shader> CurShader_; 
        Ref<Geometry> CurGeom_; 
        Ref<Texture2D> CurTextures_[MaxTextureCount];
    };

}