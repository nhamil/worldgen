#include "Fjord/Graphics/Mesh.h" 

#include "Fjord/Graphics/Graphics.h" 
#include "Fjord/Graphics/IndexBuffer.h"
#include "Fjord/Graphics/Geometry.h"  
#include "Fjord/Graphics/VertexBuffer.h"
#include "Fjord/Util/Memory.h" 

namespace Fjord 
{

    bool MeshData::Apply(Mesh* mesh) const 
    {
        FJ_EASSERT(mesh); 
        bool success = true; 

        mesh->Clear(); 

        if (Vertices.size()) mesh->SetVertices(Vertices); 
        if (Normals.size()) mesh->SetNormals(Normals); 
        if (Tangents.size()) mesh->SetTangents(Tangents); 
        if (Colors.size()) mesh->SetColors(Colors); 
        if (TexCoords.size()) mesh->SetTexCoords(TexCoords); 
        if (Indices.size()) mesh->SetIndices(Indices); 

        mesh->Update(); 

        // TODO mesh->Set* should return whether successful or not 
        return success; 
    }

    Mesh* Mesh::CreateCube() 
    {
        Mesh* mesh = new Mesh(); 

        static const float S = 0.5f; 

        static Vector<Vector3> verts({
            // front 
            {-S,  S,  S}, 
            {-S, -S,  S}, 
            { S, -S,  S}, 
            { S,  S,  S}, 
            // back 
            { S,  S, -S}, 
            { S, -S, -S}, 
            {-S, -S, -S}, 
            {-S,  S, -S}, 
            // left 
            {-S,  S, -S}, 
            {-S, -S, -S}, 
            {-S, -S,  S}, 
            {-S,  S,  S}, 
            // right 
            { S,  S,  S}, 
            { S, -S,  S}, 
            { S, -S, -S}, 
            { S,  S, -S}, 
            // top 
            {-S,  S, -S}, 
            {-S,  S,  S}, 
            { S,  S,  S}, 
            { S,  S, -S}, 
            // bottom 
            {-S, -S,  S}, 
            {-S, -S, -S}, 
            { S, -S, -S}, 
            { S, -S,  S} 
        }); 
        static Vector<Vector4> colors({
            // front 
            Color::Yellow, 
            Color::Yellow, 
            Color::Yellow, 
            Color::Yellow, 
            // back 
            Color::Cyan, 
            Color::Cyan, 
            Color::Cyan, 
            Color::Cyan, 
            // left 
            Color::Red, 
            Color::Red, 
            Color::Red, 
            Color::Red, 
            // right 
            Color::Green, 
            Color::Green, 
            Color::Green, 
            Color::Green, 
            // top 
            Color::Blue, 
            Color::Blue, 
            Color::Blue, 
            Color::Blue, 
            // bottom 
            Color::Fuchsia, 
            Color::Fuchsia, 
            Color::Fuchsia, 
            Color::Fuchsia, 
        });
        static Vector<uint32> inds({
            0, 1, 2, 0, 2, 3, 
            4, 5, 6, 4, 6, 7, 
            8, 9, 10, 8, 10, 11, 
            12, 13, 14, 12, 14, 15, 
            16, 17, 18, 16, 18, 19, 
            20, 21, 22, 20, 22, 23
        }); 

        mesh->SetVertices(verts); 
        mesh->SetColors(colors); 
        mesh->SetIndices(inds); 
        mesh->CalculateNormals(); 
        mesh->Update(); 

        return mesh; 
    }

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

    Primitive Mesh::GetPrimitive() const 
    {
        return Primitive_; 
    }

    void Mesh::SetPrimitive(Primitive prim) 
    {
        Primitive_ = prim; 
    }

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

    bool Mesh::HasIndices() const 
    {
        return (bool) IndexBuffer_; 
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

    void Mesh::CalculateNormals() 
    {
        FJ_EASSERT(HasVertices()); 
        FJ_EASSERT(HasIndices()); 
        FJ_EASSERT(GetPrimitive() == Primitive::Triangles); 

        unsigned vertCount = GetVertexCount(); 
        unsigned indCount = GetIndexCount(); 

        Vector<Vector3> norm(vertCount); 
        Vector<uint32> inds = GetIndices(); 
        Vector<Vector3> verts = GetVertices(); 
        Vector3 a, b, c; 

        for (unsigned i = 0; i < indCount; i += 3) 
        {
            unsigned i1 = inds[i + 0]; 
            unsigned i2 = inds[i + 1]; 
            unsigned i3 = inds[i + 2]; 

            a = verts[i1]; 
            b = verts[i2]; 
            c = verts[i3]; 

            b = Normalized(b - a); 
            c = Normalized(c - a); 

            b = Normalized(Cross(b, c)); 

            norm[i1] += b; 
            norm[i2] += b; 
            norm[i3] += b; 
        }

        for (unsigned i = 0; i < vertCount; i++) 
        {
            norm[i] = Normalized(norm[i]); 
        }

        SetNormals(norm); 
    }

    void Mesh::CalculateTangents() 
    {
        FJ_EASSERT(HasVertices()); 
        FJ_EASSERT(HasNormals()); 
        FJ_EASSERT(HasTexCoords()); 
        FJ_EASSERT(HasIndices()); 
        FJ_EASSERT(GetPrimitive() == Primitive::Triangles); 

        unsigned vertCount = GetVertexCount(); 
        unsigned indCount = GetIndexCount(); 

        Vector<uint32> inds = GetIndices(); 
        Vector<Vector3> verts = GetVertices(); 
        Vector<Vector3> norm = GetNormals(); 
        Vector<Vector2> texCoords = GetTexCoords(); 

        Vector<Vector3> tan1(vertCount); 
        // Vector<Vector3> tan2(vertCount); // TODO why is this here 

        Vector3 v1, v2, v3; 
        Vector2 w1, w2, w3; 

        Vector3 sDir;//, tDir; 

        for (unsigned i = 0; i < indCount; i += 3) 
        {
            unsigned i1 = inds[i + 0]; 
            unsigned i2 = inds[i + 1]; 
            unsigned i3 = inds[i + 2]; 

            v1 = verts[i1]; 
            v2 = verts[i2]; 
            v3 = verts[i3]; 

            w1 = texCoords[i1]; 
            w2 = texCoords[i2]; 
            w3 = texCoords[i3]; 

            float x1 = v2.X - v1.X; 
            float x2 = v3.X - v1.X; 
            float y1 = v2.Y - v1.Y; 
            float y2 = v3.Y - v1.Y; 
            float z1 = v2.Z - v1.Z; 
            float z2 = v3.Z - v1.Z; 

            float s1 = w2.X - w1.X; 
            float s2 = w3.X - w1.X; 
            float t1 = w2.Y - w1.Y; 
            float t2 = w3.Y - w1.Y; 

            float r = 1.0f / (s1 * t2 - s2 * t1); 

            sDir = {
                r * (t2 * x1 - t1 * x2), 
                r * (t2 * y1 - t1 * y2), 
                r * (t2 * z1 - t1 * z2) 
            };
            // tDir = {
            //     r * (s1 * x2 - s2 * x1), 
            //     r * (s1 * y2 - s2 * y1), 
            //     r * (s1 * z2 - s2 * z1) 
            // };

            tan1[i1] += sDir; 
            tan1[i2] += sDir; 
            tan1[i3] += sDir; 
            // tan2[i1] += tDir; 
            // tan2[i2] += tDir; 
            // tan2[i3] += tDir; 
        }

        for (unsigned i = 0; i < vertCount; i++) 
        {
            tan1[i] = Normalized(tan1[i]); 
            // tan2[i] = Normalized(tan2[i]); 
        }

        SetTangents(tan1); 
    }

}