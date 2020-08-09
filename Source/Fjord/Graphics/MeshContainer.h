#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Mesh.h" 
#include "Fjord/Graphics/Material.h" 
#include "Fjord/Scene/Component.h" 

namespace Fjord 
{

    class MeshContainer : public Component 
    {
    public: 
        Mesh* GetMesh() const { return Mesh_; } 
        Material* GetMaterial() const { return Material_; } 

        void SetMesh(Mesh* mesh) { Mesh_ = mesh; } 
        void SetMaterial(Material* mat) { Material_ = mat; } 

    private: 
        Ref<Mesh> Mesh_; 
        Ref<Material> Material_; 
    };

}