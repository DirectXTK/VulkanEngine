#pragma once
#include "RendCore.h"
#include "Context.h"

class DescriptorLayoudwadt{
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
struct DescriptorSetDescription {
    Context context{};
    uint32_t DescriptorCount{};
    VkDescriptorPool DescriptorPool{};
    VkDescriptorType Type{};
    VkShaderStageFlags StageFlags{};
};
class DescriptorSet {
public:
    void Init(DescriptorSetDescription desc);

    VkDescriptorSet GetDescriptorSet() { return m_DescriptorSet; }
    VkDescriptorSetLayout GetDescriptorLayout() { return m_DescriptorSetLayout; }
    uint32_t GetDescriptorCount() { return m_DescriptorCount; }

    //Write Fn
    void WriteTo(uint32_t Index, VkBuffer uniformBuffer, uint64_t Size);
    void WriteToTexture(uint32_t Index, VkImageView imageView, VkSampler sampler);
private:
    void CreateDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout, uint32_t descriptorCount, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags);

    Context m_Context{};
    VkDescriptorSet m_DescriptorSet{};
    VkDescriptorSetLayout m_DescriptorSetLayout{};
    uint32_t m_DescriptorCount;
};
class DescriptorPool {
public:
    void AddDescriptorType(uint32_t Count,VkDescriptorType type);
    void CreatePool(Context context);

    VkDescriptorPool GetPool() { return m_Pool; }
    uint32_t GetPoolSize() { return m_DecriptorPoolSize; }

private:
    VkDescriptorPool m_Pool{};
    uint32_t m_DecriptorPoolSize{};

    struct DescriptorPoolSizeCreateInfo {
        uint32_t Count{};
        VkDescriptorType Type{};
    };
    std::vector<DescriptorPoolSizeCreateInfo> m_DescriptorPoolSizeInfo{};
};
