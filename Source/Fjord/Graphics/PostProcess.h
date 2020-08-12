#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Graphics/Color.h" 
#include "Fjord/Graphics/RenderTargetSwap.h" 
#include "Fjord/Graphics/Shader.h" 
#include "Fjord/Util/ClassId.h" 

namespace Fjord 
{

    class PostProcessPipeline; 
    class RenderTargetSwap; 

    class PostProcessEffect : public RefCounted 
    {
    public: 
        virtual void Apply(RenderTargetSwap* target) = 0; 
    };

    class PostProcessPipeline : public RefCounted 
    {
    public: 
        void Apply(RenderTargetSwap* target); 

        template <class T> 
        T* AddEffect() 
        {
            T* old = GetEffect<T>(); 
            if (old) 
            {
                return old; 
            }
            else 
            {
                PostProcessEffect* effect = new T(); 
                Effects_.push_back(effect); 
                IdToEffect_[GetClassId<T>()] = effect; 
                return (T*) effect; 
            }
        } 

        template <class T> 
        T* GetEffect() 
        {
            auto it = IdToEffect_.find(GetClassId<T>()); 
            if (it != IdToEffect_.end()) 
            {
                return (T*) (it->second.Get()); 
            }
            else 
            {
                return nullptr; 
            }
        }

        unsigned GetEffectCount() 
        {
            return Effects_.size(); 
        }

        PostProcessEffect* GetEffect(unsigned index) 
        {
            FJ_EASSERT(index < Effects_.size()); 
            return Effects_[index]; 
        }

    private: 
        Vector<Ref<PostProcessEffect>> Effects_; 
        HashMap<ClassId, Ref<PostProcessEffect>> IdToEffect_; 
    };

    class BloomEffect : public PostProcessEffect
    {
    public: 
        BloomEffect(); 
        
        const Color& GetColor() const { return Color_; } 
        void SetColor(const Color& color) { Color_ = color; } 

        float GetStrength() const { return Strength_; } 
        void SetStrength(float strength) { Strength_ = strength; } 

        float GetThreshold() const { return Threshold_; } 
        void SetThreshold(float threshold) { Threshold_ = threshold; } 

        virtual void Apply(RenderTargetSwap* target) override; 

    private: 
        static Ref<Shader> ThresholdShader_, BloomShader_, AddShader_; 
        static Ref<RenderTargetSwap> RTSwap_; 

        Color Color_{1.0, 1.0, 1.0}; 
        float Strength_ = 5.0; 
        float Threshold_ = 0.9; 
    };

}