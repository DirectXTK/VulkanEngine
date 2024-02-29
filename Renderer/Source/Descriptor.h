#pragma once
#include "RendCore.h"

class DescriptorLayout{
public:
    void Init(VkDevice device, VkDescriptorType type, VkShaderStageFlags stageflags);

    VkDescriptorSet* GetDescriptorSet(){return &m_DescriptorSet;}
    VkDescriptorSetLayout* GetDescriptorLayout(){return &m_Layout;}
    void WriteTo(VkDevice device,VkBuffer uniformbuffer,uint32_t Size);
    void WriteToTexture(VkDevice device,  VkImageView imageview, VkSampler sampler);

 private:
    VkDescriptorSetLayout m_Layout{};
    VkDescriptorPool m_DescriptorPool{};
    VkDescriptorSet m_DescriptorSet{};
};