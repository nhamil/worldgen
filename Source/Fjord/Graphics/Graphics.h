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

        void ResetViewport(); 
        void SetViewport(int x, int y, int w, int h); 

        void ResetClip(); 
        void SetClip(int x, int y, int w, int h); 

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