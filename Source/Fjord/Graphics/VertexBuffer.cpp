#include "Fjord/Graphics/VertexBuffer.h" 

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/Memory.h" 

namespace Fjord 
{

    const VertexFormat VertexFormat::Position = VertexFormat().Add(Attribute::Position); 
    const VertexFormat VertexFormat::Normal = VertexFormat().Add(Attribute::Normal); 
    const VertexFormat VertexFormat::Tangent = VertexFormat().Add(Attribute::Tangent); 
    const VertexFormat VertexFormat::Color0 = VertexFormat().Add(Attribute::Color0); 
    const VertexFormat VertexFormat::Texture0 = VertexFormat().Add(Attribute::Texture0); 
    const VertexFormat VertexFormat::PositionColor = VertexFormat().Add(Attribute::Position).Add(Attribute::Color0); 
    const VertexFormat VertexFormat::PositionTexture = VertexFormat().Add(Attribute::Position).Add(Attribute::Texture0); 
    const VertexFormat VertexFormat::PositionTextureColor = VertexFormat().Add(Attribute::Position).Add(Attribute::Texture0).Add(Attribute::Color0); 

    VertexBuffer::VertexBuffer(const VertexFormat& format, unsigned numElements)  
        : Format_(format) 
    {
        // FJ_EFDEBUG("ctor"); 
        GLCALL(glGenBuffers(1, &Handle_)); 
        SetElementCount(numElements); 
    }

    VertexBuffer::~VertexBuffer() 
    {
        if (Handle_ != 0) 
        {
            GLCALL(glDeleteBuffers(1, &Handle_)); 
            Handle_ = 0; 
        }
        // FJ_EFDEBUG("dtor"); 
    }

    const VertexFormat& VertexBuffer::GetFormat() const 
    {
        return Format_; 
    }

    unsigned VertexBuffer::GetElementCount() const 
    {
        return Data_.size() / Format_.GetVertexByteSize(); 
    }

    void VertexBuffer::SetElementCount(unsigned num) 
    {
        Data_.resize(num * Format_.GetVertexByteSize()); 
        Dirty_ = true; 
    }

    void VertexBuffer::SetData(unsigned start, unsigned count, const void* in) 
    {
        FJ_EASSERT(start + count <= GetElementCount()) 

        unsigned s = start * Format_.GetVertexByteSize(); 

        MemCpy(&Data_[s], in, count, Format_.GetVertexByteSize()); 
        Dirty_ = true; 
    }

    void VertexBuffer::GetData(unsigned start, unsigned count, void* out) const 
    {
        FJ_EASSERT(start + count <= GetElementCount()) 

        unsigned s = start * Format_.GetVertexByteSize(); 

        MemCpy(out, &Data_[s], count, Format_.GetVertexByteSize()); 
    }

    void VertexBuffer::Update() 
    {
        if (!Dirty_) return; 

        FJ_EASSERT(Handle_ != 0); 

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, Handle_)); 
        GLCALL(glBufferData(
            GL_ARRAY_BUFFER, 
            GetElementCount() * Format_.GetVertexByteSize(), 
            &Data_[0], 
            GL_DYNAMIC_DRAW 
        )); 

        Dirty_ = false; 
    }

}