#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Scene/Component.h" 

namespace Fjord 
{

    enum class Projection 
    {
        Perspective
    };

    class Camera : public Component 
    {
    public: 
        Projection GetProjection() const { return Proj_; } 
        float GetFOV() const { return FOV_; } 

        void SetFOV(float fov) { FOV_ = fov; } 

    private: 
        Projection Proj_ = Projection::Perspective; 
        float FOV_ = 70.0; 
    };

}