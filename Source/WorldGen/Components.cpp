#include "WorldGen/Components.h" 

LODSphere::LODSphere() 
{
    for (unsigned i = 0; i < 6; i++) 
    {
        Nodes[i] = nullptr; 
    }
}

LODSphere::~LODSphere() 
{
    for (unsigned i = 0; i < 6; i++) 
    {
        Nodes[i] = nullptr; 
    }
}