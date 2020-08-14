#pragma once 

#include <Fjord/Fjord.h> 

#include "WorldGen/World.h" 
#include "WorldGen/WorldGenThread.h" 
#include "WorldGen/LOD/FaceNode.h" 

using namespace Fjord; 

class FPSCamera : public Component 
{
public: 
    float RotX = 0.0f; // up-down 
    float RotY = 0.0f; // left-right 
}; 

class RotateTag : public Component 
{
public: 
    float Amount = 1.0f;  
}; 

class Orbit : public Component 
{
public: 
    Entity Orbits = Scene::NullEntity; 
    float Distance = 10.0f; 
    float Angle = 0.0f; 
    float Speed = 10.0f; 
};

class OrbitCamera : public Component 
{
    float RotX = 0.0f; // up-down 
    float RotY = 0.0f; // left-right 
    float Zoom = 1.0f; 
};

class Planet : public Component 
{
public: 
    bool Empty = true; 
    Ref<World> World; 
    Ref<WorldGenThread> GenThread; 
    Ref<Material> MainMaterial; 
    Ref<Mesh> CellMesh, OutlineMesh; 
};

class LODSphere : public Component 
{
public: 
    Ref<FaceNode> Nodes[FaceIndexCount]; 

    LODSphere(); 
    ~LODSphere(); 
};