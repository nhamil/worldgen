#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/Types.h" 
#include "Fjord/Math/Vector3.h" 

#include <vector> 

namespace Fjord 
{

    struct VertexPosition 
    {
        Vector3 Position; 
    };

    struct VertexPositionColor 
    {
        Vector3 Position; 
        struct Color Color; 
    };

    struct VertexPositionTexture 
    {
        Vector3 Position; 
        Vector2 Texture; 
    };

    struct VertexPositionTextureColor 
    {
        Vector3 Position; 
        Vector2 Texture; 
        struct Color Color; 
    };

    class VertexFormat 
    {
    public: 
        static const VertexFormat Position; 
        static const VertexFormat Normal; 
        static const VertexFormat Tangent; 
        static const VertexFormat Color0; 
        static const VertexFormat Texture0; 
        
        static const VertexFormat PositionColor; 
        static const VertexFormat PositionTexture; 
        static const VertexFormat PositionTextureColor; 

        VertexFormat() = default; 
        ~VertexFormat() = default; 

        VertexFormat& Add(Attribute attrib) 
        {
            Attribs_.push_back(attrib); 
            ElemSize_ += AttributeSize[(unsigned) attrib]; 
            return *this; 
        } 

        unsigned GetAttributeCount() const { return Attribs_.size(); } 

        Attribute GetAttribute(unsigned index) const { return Attribs_[index]; }

        unsigned GetVertexSize() const { return ElemSize_; } 

        unsigned GetVertexByteSize() const { return ElemSize_ * sizeof(float); } 

        int GetOffset(Attribute attrib) const 
        {
            int offset = 0; 
            for (Attribute a : Attribs_) 
            {
                if (a == attrib) return offset; 
                offset += AttributeSize[(unsigned) a]; 
            }
            return -1; 
        }

        int GetByteOffset(Attribute attrib) const { return GetOffset(attrib) * sizeof(float); } 

    private: 
        unsigned ElemSize_ = 0; 
        Vector<Attribute> Attribs_; 
    };

}