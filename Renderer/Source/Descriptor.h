#pragma once
#include "RendCore.h"

class DescriptorLayout{
public:
    void Init(VkDevice device);

    VkDescriptorSet* GetDescriptorSet(){return &m_DescriptorSet;}
    VkDescriptorSetLayout* GetDescriptorLayout(){return &m_Layout;}
    void WriteTo(VkDevice device,VkBuffer uniformbuffer,uint32_t Size);
 private:
    VkDescriptorSetLayout m_Layout{};
    VkDescriptorPool m_DescriptorPool{};
    VkDescriptorSet m_DescriptorSet{};
};