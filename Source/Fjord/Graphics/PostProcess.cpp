#include "Fjord/Graphics/PostProcess.h" 

#include "Fjord/Graphics/Graphics.h" 

namespace Fjord 
{

    Ref<Shader> BloomEffect::ThresholdShader_; 
    Ref<Shader> BloomEffect::BloomShader_; 
    Ref<Shader> BloomEffect::AddShader_; 
    Ref<RenderTargetSwap> BloomEffect::RTSwap_; 

    void PostProcessPipeline::Apply(RenderTargetSwap* target) 
    {
        for (auto effect : Effects_) 
        {
            effect->Apply(target); 
        }
    }

    BloomEffect::BloomEffect() 
    {
        if (!RTSwap_) 
        {
            ThresholdShader_ = Shader::Load("Threshold"); 
            BloomShader_ = Shader::Load("Bloom"); 
            AddShader_ = Shader::Load("Add"); 
            RTSwap_ = new RenderTargetSwap(); 
        }
    }

    void BloomEffect::Apply(RenderTargetSwap* target) 
    {
        auto* graphics = GetGraphics(); 

        if (Strength_ <= 0.0) return; 

        float scale = 1.0; 
        RTSwap_->SetSize(target->GetWidth() * scale, target->GetHeight() * scale); 
        RTSwap_->Clear(); 

        ThresholdShader_->SetTextureUnit("u_Texture", 0); 
        ThresholdShader_->SetInt("u_Threshold", GetThreshold()); 
        graphics->SetTexture(0, target->GetSource()->GetColorBuffer(0)); 
        target->ApplyEffect(target->GetSource(), RTSwap_->GetDest(), ThresholdShader_); 
        RTSwap_->Swap(); 

        BloomShader_->SetTextureUnit("u_Texture", 0); 
        BloomShader_->SetInt("u_Horizontal", 0); 
        BloomShader_->SetInt("u_Strength", Max<int>(Strength_ * scale * RTSwap_->GetDest()->GetHeight() / 600, 1)); 
        BloomShader_->SetVector4("u_Color", Color_); 
        graphics->SetTexture(0, RTSwap_->GetSource()->GetColorBuffer(0)); 
        target->ApplyEffect(RTSwap_->GetSource(), RTSwap_->GetDest(), BloomShader_); 
        RTSwap_->Swap(); 

        BloomShader_->SetTextureUnit("u_Texture", 0); 
        BloomShader_->SetInt("u_Horizontal", 1); 
        graphics->SetTexture(0, RTSwap_->GetSource()->GetColorBuffer(0)); 
        target->ApplyEffect(RTSwap_->GetSource(), RTSwap_->GetDest(), BloomShader_); 
        RTSwap_->Swap(); 

        AddShader_->SetTextureUnit("u_TextureBase", 0); 
        AddShader_->SetTextureUnit("u_TextureAdd", 1); 
        graphics->SetTexture(0, target->GetSource()->GetColorBuffer(0)); 
        graphics->SetTexture(1, RTSwap_->GetSource()->GetColorBuffer(0)); 
        target->ApplyEffect(RTSwap_->GetSource(), target->GetDest(), AddShader_); 
        target->Swap(); 
    }

}