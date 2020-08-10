#include "Fjord/Graphics/IndexBuffer.h" 

#include "Fjord/Graphics/OpenGL.h" 
#include "Fjord/Util/Memory.h" 

namespace Fjord 
{

    IndexBuffer::IndexBuffer(unsigned numElements) 
    {
        // FJ_EFDEBUG("ctor"); 
        GLCALL(glGenBuffers(1, &Handle_)); 
        SetElementCount(numElements); 
    }

    IndexBuffer::~IndexBuffer() 
    {
        if (Handle_ != 0) 
        {
            // GLCALL(glDeleteBuffers(1, &Handle_)); 
            GetAPI()->DeleteIndexBuffer(Handle_); 
            Handle_ = 0; 
        }
        // FJ_EFDEBUG("dtor"); 
    }

    unsigned IndexBuffer::GetElementCount() const 
    {
        return Data_.size(); 
    }

    void IndexBuffer::SetElementCount(unsigned num) 
    {
        Data_.resize(num); 
        Dirty_ = true; 
    }

    void IndexBuffer::SetData(unsigned start, unsigned count, const uint32* in) 
    {
        FJ_EASSERT(start + count <= GetElementCount()) 

        MemCpy(&Data_[start], in, count, sizeof(uint32)); 
        Dirty_ = true; 
    }

    void IndexBuffer::GetData(unsigned start, unsigned count, uint32* out) const 
    {
        FJ_EASSERT(start + count <= GetElementCount()) 

        MemCpy(out, &Data_[start], count, sizeof(uint32)); 
    }

    void IndexBuffer::Update() 
    {
        if (!Dirty_) return; 

        FJ_EASSERT(Handle_ != 0); 

        // GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Handle_)); 
        GetAPI()->BindIndexbuffer(Handle_, false); 
        GLCALL(glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, 
            GetElementCount() * sizeof(uint32), 
            &Data_[0], 
            GL_DYNAMIC_DRAW 
        )); 

        Dirty_ = false; 
    }

}