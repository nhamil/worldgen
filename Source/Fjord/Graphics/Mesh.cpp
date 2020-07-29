#include "Fjord/Graphics/Mesh.h" 

#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/Graphics/IndexBuffer.h"
#include "Fjord/Graphics/Geometry.h"  
#include "Fjord/Graphics/VertexBuffer.h"
#include "Fjord/Util/Memory.h" 

namespace Fjord 
{

    Mesh::Mesh() 
    {
        Geometry_ = new Geometry(); 
        // FJ_EFDEBUG("ib %p", IndexBuffer_.Get()); 
        // for (unsigned i = 0; i < (unsigned) Attribute::count; i++) 
        // {
        //     FJ_EFDEBUG("vb %p", VertexBuffers_[i].Get()); 
        // }
    }

    Mesh::~Mesh() {}

    unsigned Mesh::GetIndexCount() const 
    {
        if (IndexBuffer_) 
        {
            return IndexBuffer_->GetElementCount(); 
        }
        else 
        {
            return 0; 
        }
    }

    unsigned Mesh::GetVertexCount() const 
    {
        if (HasVertices()) 
        {
            return VertexBuffers_[(unsigned) Attribute::Position]->GetElementCount(); 
        }
        else
        {
            return 0; 
        }
    }

    Vector<uint32> Mesh::GetIndices() const 
    {
        if (IndexBuffer_) 
        {
            Vector<uint32> inds; 
            unsigned count = IndexBuffer_->GetElementCount(); 
            inds.resize(count); 
            IndexBuffer_->GetData(0, count, &inds[0]); 
            return inds; 
        }
        else 
        {
            return Vector<uint32>(); 
        }
    }

    Vector<Vector3> Mesh::GetVertices() const 
    {
        if (HasVertices()) 
        {
            auto vb = VertexBuffers_[(unsigned) Attribute::Position]; 
            Vector<Vector3> list; 
            list.resize(GetVertexCount()); 
            vb->GetData(0, GetVertexCount(), &list[0]); 
            return list; 
        }
        else 
        {
            return Vector<Vector3>(); 
        }
    } 

    Vector<Vector3> Mesh::GetNormals() const 
    {
        if (HasNormals()) 
        {
            auto vb = VertexBuffers_[(unsigned) Attribute::Normal]; 
            Vector<Vector3> list; 
            list.resize(GetVertexCount()); 
            vb->GetData(0, GetVertexCount(), &list[0]); 
            return list; 
        }
        else 
        {
            return Vector<Vector3>(); 
        }
    } 

    Vector<Vector3> Mesh::GetTangents() const 
    {
        if (HasTangents()) 
        {
            auto vb = VertexBuffers_[(unsigned) Attribute::Tangent]; 
            Vector<Vector3> list; 
            list.resize(GetVertexCount()); 
            vb->GetData(0, GetVertexCount(), &list[0]); 
            return list; 
        }
        else 
        {
            return Vector<Vector3>(); 
        }
    } 

    Vector<Vector4> Mesh::GetColors() const 
    {
        if (HasColors()) 
        {
            auto vb = VertexBuffers_[(unsigned) Attribute::Color0]; 
            Vector<Vector4> list; 
            list.resize(GetVertexCount()); 
            vb->GetData(0, GetVertexCount(), &list[0]); 
            return list; 
        }
        else 
        {
            return Vector<Vector4>(); 
        }
    } 

    Vector<Vector2> Mesh::GetTexCoords() const 
    {
        if (HasTexCoords()) 
        {
            auto vb = VertexBuffers_[(unsigned) Attribute::Texture0]; 
            Vector<Vector2> list; 
            list.resize(GetVertexCount()); 
            vb->GetData(0, GetVertexCount(), &list[0]); 
            return list; 
        }
        else 
        {
            return Vector<Vector2>(); 
        }
    } 

    bool Mesh::HasVertices() const 
    {
        return (bool) VertexBuffers_[(unsigned) Attribute::Position]; 
    } 

    bool Mesh::HasNormals() const 
    {
        return (bool) VertexBuffers_[(unsigned) Attribute::Normal]; 
    } 

    bool Mesh::HasTangents() const 
    {
        return (bool) VertexBuffers_[(unsigned) Attribute::Tangent]; 
    } 

    bool Mesh::HasColors() const 
    {
        return (bool) VertexBuffers_[(unsigned) Attribute::Color0]; 
    }

    bool Mesh::HasTexCoords() const 
    {
        return (bool) VertexBuffers_[(unsigned) Attribute::Texture0]; 
    }

    void Mesh::SetIndices(const Vector<uint32>& indices) 
    { 
        if (indices.size() > 0) 
        {
            SetIndices(indices.size(), &indices[0]); 
        }
        else 
        {
            ClearIndices(); 
        }
    } 

    void Mesh::SetVertices(const Vector<Vector3>& list) 
    {
        if (list.size() > 0) 
        {
            SetVertices(list.size(), &list[0]); 
        }
        else 
        {
            Clear(); 
        }
    } 

    void Mesh::SetNormals(const Vector<Vector3>& list) 
    {
        if (list.size() > 0) 
        {
            SetNormals(list.size(), &list[0]); 
        }
        else 
        {
            ClearNormals(); 
        }
    } 
 
    void Mesh::SetTangents(const Vector<Vector3>& list) 
    {
        if (list.size() > 0) 
        {
            SetTangents(list.size(), &list[0]); 
        }
        else 
        {
            ClearTangents(); 
        }
    } 
 
    void Mesh::SetColors(const Vector<Vector4>& list) 
    {
        if (list.size() > 0) 
        {
            SetColors(list.size(), &list[0]); 
        }
        else 
        {
            ClearColors(); 
        }
    } 
 
    void Mesh::SetTexCoords(const Vector<Vector2>& list) 
    {
        if (list.size() > 0) 
        {
            SetTexCoords(list.size(), &list[0]); 
        }
        else 
        {
            ClearTexCoords(); 
        }
    } 
 
    void Mesh::SetIndices(unsigned count, const uint32* list) 
    {
        if (count == 0 || list == nullptr) 
        {
            ClearIndices(); 
            return; 
        }

        unsigned maxInd = GetVertexCount(); 
        for (unsigned i = 0; i < count; i++) 
        {
            if (list[i] >= maxInd) 
            {
                FJ_EFLOG(Error, "Index out of bounds, removing index buffer"); 
                IndexBuffer_ = nullptr; 
                return; 
            }
        }

        if (!IndexBuffer_) IndexBuffer_ = new IndexBuffer(count); 
        IndexBuffer_->SetData(0, count, list); 
    }

    void Mesh::SetVertices(unsigned count, const Vector3* list) 
    {
        if (count == 0 || list == nullptr) 
        {
            Clear(); 
            return; 
        }
        auto vb = GetOrMakeVertexBuffer(Attribute::Position, true); 
        vb->SetElementCount(count); 
        vb->SetData(0, count, list); 

        bool resize = false; 
        for (unsigned i = 0; i < (unsigned) Attribute::count; i++) 
        {
            auto buffer = VertexBuffers_[i]; 
            if (buffer) 
            {
                if (buffer->GetElementCount() != count) 
                {
                    resize = true; 
                    buffer->SetElementCount(count); 
                }
            }
        }
        if (resize) FJ_EFLOG(Debug, "Resized non-position vertex buffers"); 

        if (IndexBuffer_) 
        {
            uint32* inds = new uint32[IndexBuffer_->GetElementCount()]; 
            for (unsigned i = 0; i < IndexBuffer_->GetElementCount(); i++) 
            {
                if (inds[i] >= count) 
                {
                    FJ_EFLOG(Error, "Index out of bounds, removing index buffer"); 
                    IndexBuffer_ = nullptr; 
                    break; 
                }
            }
            delete[] inds; 
        }
    } 

    void Mesh::SetNormals(unsigned count, const Vector3* list) 
    {
        if (count != GetVertexCount()) 
        {
            FJ_EFLOG(Error, "Wrong number of vertices, removing buffer"); 
            ClearNormals(); 
            return; 
        }

        if (count == 0 || list == nullptr) 
        {
            ClearNormals(); 
            return; 
        }

        auto vb = GetOrMakeVertexBuffer(Attribute::Normal, true); 
        vb->SetElementCount(count); 
        vb->SetData(0, count, list); 
    } 

    void Mesh::SetTangents(unsigned count, const Vector3* list) 
    {
        if (count != GetVertexCount()) 
        {
            FJ_EFLOG(Error, "Wrong number of vertices, removing buffer"); 
            ClearTangents(); 
            return; 
        }

        if (count == 0 || list == nullptr) 
        {
            ClearTangents(); 
            return; 
        }

        auto vb = GetOrMakeVertexBuffer(Attribute::Tangent, true); 
        vb->SetElementCount(count); 
        vb->SetData(0, count, list); 
    } 

    void Mesh::SetColors(unsigned count, const Vector4* list) 
    {
        if (count != GetVertexCount()) 
        {
            FJ_EFLOG(Error, "Wrong number of vertices, removing buffer"); 
            ClearColors(); 
            return; 
        }

        if (count == 0 || list == nullptr) 
        {
            ClearColors(); 
            return; 
        }

        auto vb = GetOrMakeVertexBuffer(Attribute::Color0, true); 
        vb->SetElementCount(count); 
        vb->SetData(0, count, list); 
    } 

    void Mesh::SetTexCoords(unsigned count, const Vector2* list) 
    {
        if (count != GetVertexCount()) 
        {
            FJ_EFLOG(Error, "Wrong number of vertices, removing buffer"); 
            ClearTexCoords(); 
            return; 
        }

        if (count == 0 || list == nullptr) 
        {
            ClearTexCoords(); 
            return; 
        }

        auto vb = GetOrMakeVertexBuffer(Attribute::Texture0, true); 
        vb->SetElementCount(count); 
        vb->SetData(0, count, list); 
    } 

    void Mesh::Clear() 
    {
        IndexBuffer_ = nullptr; 
        for (unsigned i = 0; i < (unsigned) Attribute::count; i++) 
        {
            VertexBuffers_[i] = nullptr; 
        }
    } 
 
    void Mesh::ClearIndices() 
    {
        IndexBuffer_ = nullptr; 
    } 
 
    void Mesh::ClearNormals() 
    {
        GetOrMakeVertexBuffer(Attribute::Normal, false); 
    } 
 
    void Mesh::ClearTangents() 
    {
        GetOrMakeVertexBuffer(Attribute::Tangent, false); 
    } 

    void Mesh::ClearColors() 
    {
        GetOrMakeVertexBuffer(Attribute::Color0, false); 
    } 
 
    void Mesh::ClearTexCoords() 
    {
        GetOrMakeVertexBuffer(Attribute::Texture0, false); 
    } 

    static const VertexFormat& GetFormatForAttribute(Attribute a) 
    {
        static VertexFormat defaultFormat; 
        switch (a) 
        {
            case Attribute::Position: return VertexFormat::Position; 
            case Attribute::Normal: return VertexFormat::Normal; 
            case Attribute::Tangent: return VertexFormat::Tangent; 
            case Attribute::Color0: return VertexFormat::Color0; 
            case Attribute::Texture0: return VertexFormat::Texture0; 
            default: 
                FJ_EFLOG(Error, "Unsupported attribute: %d", a); 
                return defaultFormat; 
        }
    }

    VertexBuffer* Mesh::GetOrMakeVertexBuffer(Attribute attrib, bool create) 
    {
        unsigned ind = (unsigned) attrib; 

        if (create) 
        {
            if (!VertexBuffers_[ind]) VertexBuffers_[ind] = new VertexBuffer(GetFormatForAttribute(attrib), GetVertexCount()); 
        }
        else 
        {
            if (VertexBuffers_[ind]) VertexBuffers_[ind] = nullptr; 
        }

        return VertexBuffers_[ind]; 
    }

    void Mesh::Update() 
    {
        if (IndexBuffer_) 
        {
            IndexBuffer_->Update(); 
            Geometry_->SetIndexBuffer(IndexBuffer_); 
        }
        else 
        {
            Geometry_->SetIndexBuffer(nullptr); 
        }

        Vector<VertexBuffer*> buffers; 

        for (unsigned i = 0; i < (unsigned) Attribute::count; i++) 
        {
            if (VertexBuffers_[i]) 
            {
                VertexBuffers_[i]->Update(); 
                buffers.push_back(VertexBuffers_[i]); 
            }
        }

        Geometry_->SetVertexBuffers(buffers); 
        Geometry_->Update(); 
    } 

}