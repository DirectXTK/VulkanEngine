#include "Descriptor.h"

 


    void DescriptorSet::Init(Context context,uint32_t DescriptorCount, VkDescriptorPool descriptorPool, VkDescriptorType type, VkShaderStageFlags stageFlags)
    {
        m_Context = context;

        VkDescriptorSetLayoutBinding binding{};
        binding.descriptorType = type;
        binding.descriptorCount = DescriptorCount;
        binding.binding = 0;
        binding.stageFlags = stageFlags;

        VkDescriptorSetLayoutCreateInfo createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createinfo.bindingCount = 1;
        createinfo.pBindings = &binding;

        VkResult result = vkCreateDescriptorSetLayout(m_Context->Device, &createinfo, nullptr, &m_DescriptorSetLayout);
        if (result != VK_SUCCESS)
            Core::Log(ErrorType::Error, "Failed to create descriptor set layout.");
  

        VkDescriptorSetAllocateInfo allocinfo{};
        allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocinfo.descriptorSetCount = 1;
        allocinfo.descriptorPool = descriptorPool;
        allocinfo.pSetLayouts = &m_DescriptorSetLayout;

        result = vkAllocateDescriptorSets(m_Context->Device, &allocinfo, &m_DescriptorSet);
        if (result != VK_SUCCESS)
            Core::Log(ErrorType::Error, "Failed to allocate descritor set.");
    }

    void DescriptorSet::WriteTo(uint32_t Index, VkBuffer uniformBuffer, uint64_t Size)
    {
        VkDescriptorBufferInfo info{};
        info.buffer = uniformBuffer;
        info.offset = 0;
        info.range = Size;



        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.dstSet = m_DescriptorSet;
        write.dstArrayElement = Index;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.pBufferInfo = &info;

        //  write.pBufferInfo
        vkUpdateDescriptorSets(m_Context->Device, 1, &write, 0, nullptr);
    }

    void DescriptorSet::WriteToTexture(uint32_t Index, VkImageView imageView, VkSampler sampler)
    {
        VkDescriptorImageInfo info{};
        info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        info.imageView = imageView;
        info.sampler = sampler;



        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstSet = m_DescriptorSet;
        write.dstArrayElement = Index;
        write.dstBinding = 0;
        write.descriptorCount = 1;
        write.pImageInfo = &info;


        //  write.pBufferInfo
        vkUpdateDescriptorSets(m_Context->Device, 1, &write, 0, nullptr);
    }

    void DescriptorSet::CreateDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout,uint32_t descriptorCount,VkDescriptorType descriptorType,VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding bindings{};
        bindings.binding = 0;
        bindings.descriptorCount = descriptorCount;
        bindings.descriptorType = descriptorType;
        bindings.stageFlags = stageFlags;


        VkDescriptorSetLayoutCreateInfo createinfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        createinfo.bindingCount = 1;
        createinfo.pBindings = &bindings;

        VkResult result = vkCreateDescriptorSetLayout(m_Context->Device, &createinfo, nullptr, descriptorSetLayout);
        if (result != VK_SUCCESS)
            Core::Log(ErrorType::Error, "Failed to create DescriptorSetLayout.");
    }




    void DescriptorPool::AddDescriptorType(uint32_t Count,VkDescriptorType type)
    {

        m_DescriptorPoolSizeInfo.push_back({ Count,type });
    }

    void DescriptorPool::CreatePool(Context context)
    {
        VkDescriptorPoolSize* PoolSizes{};
        PoolSizes = new VkDescriptorPoolSize[m_DescriptorPoolSizeInfo.size()];
        for (uint32_t i = 0; i < m_DescriptorPoolSizeInfo.size(); i++) {
            PoolSizes[i].type = m_DescriptorPoolSizeInfo[i].Type;
            PoolSizes[i].descriptorCount = m_DescriptorPoolSizeInfo[i].Count;
        }
       

        VkDescriptorPoolCreateInfo createinfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        createinfo.pPoolSizes = PoolSizes;
        createinfo.poolSizeCount = m_DescriptorPoolSizeInfo.size();
        createinfo.maxSets = m_DescriptorPoolSizeInfo.size();

       VkResult result =  vkCreateDescriptorPool(context->Device, &createinfo, nullptr, &m_Pool);
       if (result != VK_SUCCESS)
           Core::Log(ErrorType::Error, "Failed to create DescriptorPool.");
    }
