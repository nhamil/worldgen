#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Types.h" 
#include "Fjord/Graphics/VertexFormat.h" 

namespace Fjord 
{

    class VertexBuffer : public GPUObject
    {
    public: 
        VertexBuffer(const VertexFormat& format, unsigned numElements = 0); 
        virtual ~VertexBuffer(); 

        void Update(); 

        unsigned GetElementCount() const; 
        const VertexFormat& GetFormat() const; 

        void SetElementCount(unsigned numElements); 
        void SetData(unsigned start, unsigned numElements, const void* in); 

        template <class Vertex> 
        void SetData(const Vector<Vertex>& in) 
        {
            FJ_EASSERT(sizeof(Vertex) == Format_.GetVertexByteSize()); 
            SetElementCount(in.size()); 
            // FJ_EFDEBUG("%u", in.size() * sizeof(Vertex)); 
            // FJ_EFDEBUG("%u", Data_.size() * sizeof(char)); 
            // FJ_EFDEBUG("%u", Format_.GetVertexByteSize()); 
            SetData(0, in.size(), &in[0]); 
        }

        void GetData(unsigned start, unsigned numElements, void* out) const; 

    private: 
        bool Dirty_ = true; 
        Vector<char> Data_; 
        VertexFormat Format_; 
    };

}