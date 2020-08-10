#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Types.h" 
#include "Fjord/Math/Vector2.h" 
#include "Fjord/Math/Vector3.h" 
#include "Fjord/Math/Vector4.h" 

namespace Fjord 
{

    class Mesh; 

    /** 
     * A way to store mesh data on any thread. 
     */ 
    struct MeshData : public RefCounted 
    {
        Vector<uint32> Indices; 
        Vector<Vector3> Vertices; 
        Vector<Vector3> Normals; 
        Vector<Vector3> Tangents; 
        Vector<Vector4> Colors; 
        Vector<Vector2> TexCoords; 

        /** 
         * Must be called from the main thread. 
         */ 
        bool Apply(Mesh* mesh) const; 
    }; 

    class Mesh : public RefCounted
    {
    public: 
        static Mesh* CreateCube(); 

        Mesh(); 
        ~Mesh(); 

        Vector<uint32> GetIndices() const; 
        Vector<Vector3> GetVertices() const; 
        Vector<Vector3> GetNormals() const; 
        Vector<Vector3> GetTangents() const; 
        Vector<Vector4> GetColors() const; 
        Vector<Vector2> GetTexCoords() const; 

        Primitive GetPrimitive() const; 
        RenderQueue GetRenderQueue() const; 

        unsigned GetIndexCount() const; 
        unsigned GetVertexCount() const; 
        bool HasIndices() const; 
        bool HasVertices() const; 
        bool HasNormals() const; 
        bool HasTangents() const; 
        bool HasColors() const; 
        bool HasTexCoords() const; 

        void SetIndices(const Vector<uint32>& indices); 
        void SetVertices(const Vector<Vector3>& vertices); 
        void SetNormals(const Vector<Vector3>& normals); 
        void SetTangents(const Vector<Vector3>& tangents); 
        void SetColors(const Vector<Vector4>& colors); 
        void SetTexCoords(const Vector<Vector2>& texCoords); 

        void SetPrimitive(Primitive prim); 
        void SetRenderQueue(RenderQueue queue); 

        void Clear();   
        void ClearIndices(); 
        void ClearNormals(); 
        void ClearTangents(); 
        void ClearColors(); 
        void ClearTexCoords(); 

        void SetIndices(unsigned count, const uint32* indices); 
        void SetVertices(unsigned count, const Vector3* vertices); 
        void SetNormals(unsigned count, const Vector3* normals); 
        void SetTangents(unsigned count, const Vector3* tangents); 
        void SetColors(unsigned count, const Vector4* colors); 
        void SetTexCoords(unsigned count, const Vector2* texCoords); 

        void CalculateNormals(); 
        void CalculateTangents(); 

        void Update(); 

        Geometry* GetGeometry() const 
        {
            return Geometry_; 
        }

    private: 
        VertexBuffer* GetOrMakeVertexBuffer(Attribute attrib, bool shouldExist); 

        Primitive Primitive_ = Primitive::Triangles; 
        RenderQueue RenderQueue_ = RenderQueue::Opaque; 
        Ref<VertexBuffer> VertexBuffers_[(unsigned) Attribute::count]; 
        Ref<Geometry> Geometry_; 
        Ref<IndexBuffer> IndexBuffer_; 
    };

}