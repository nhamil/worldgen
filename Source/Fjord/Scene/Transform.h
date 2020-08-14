#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Matrix4.h" 
#include "Fjord/Math/Vector3.h" 
#include "Fjord/Math/Quaternion.h" 
#include "Fjord/Scene/Component.h" 

namespace Fjord 
{

    class Transform : public Component
    {
    public: 
        const Vector3& GetPosition() const { return Position_; } 
        const Quaternion& GetRotation() const { return Rotation_; } 
        const Vector3& GetScale() const { return Scale_; } 

        void SetPosition(const Vector3& pos) 
        {
            Position_ = pos; 
            Dirty_ = true; 
        }

        void SetRotation(const Quaternion& rot) 
        {
            Rotation_ = rot; 
            Dirty_ = true; 
        }

        void SetScale(const Vector3& scale) 
        {
            Scale_ = scale; 
            Dirty_ = true; 
        }

        void SetScale(float scale) 
        {
            SetScale(Vector3(scale)); 
        }

        bool IsDirty() 
        {
            return Dirty_; 
        }

        // construct a new matrix
        Matrix4 GetMatrix() 
        {
            // TODO parent 
            if (Dirty_) 
            {
                Transform_ = Matrix4::Translation(Position_); 
                Transform_ *= Matrix4::Scale(Scale_); 
                Transform_ *= Matrix4::FromQuaternion(Rotation_); 
            }
            return Transform_; 
        }

    private: 
        Matrix4 Transform_; 
        Quaternion Rotation_ = Quaternion::Identity; 
        Vector3 Position_ = Vector3::Zero; 
        Vector3 Scale_ = Vector3(1); 
        bool Dirty_ = true; 
    };

}