#include "Descriptor.h"

   void DescriptorLayout::Init(VkDevice device){
        VkDescriptorSetLayoutBinding binding{};
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount =1;
        binding.binding = 0;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createinfo.bindingCount = 1;
        createinfo.pBindings = &binding;

        VkResult result = vkCreateDescriptorSetLayout(device,&createinfo,nullptr,&m_Layout);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create descriptor set layout.");
        VkDescriptorPoolSize poolsize{};
        poolsize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolsize.descriptorCount = 1;


        VkDescriptorPoolCreateInfo poolinfo{};
        poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolinfo.maxSets = 1;
        poolinfo.poolSizeCount =1;
        poolinfo.pPoolSizes = &poolsize;

        result =vkCreateDescriptorPool(device,&poolinfo,nullptr,&m_DescriptorPool);
        if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create descriptor pool.");



        VkDescriptorSetAllocateInfo allocinfo{};
        allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocinfo.descriptorSetCount = 1;
        allocinfo.descriptorPool = m_DescriptorPool;
        allocinfo.pSetLayouts = &m_Layout;

        result = vkAllocateDescriptorSets(device,&allocinfo,&m_DescriptorSet);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to allocate descritor set.");
    }
    void DescriptorLayout::WriteTo(VkDevice device,VkBuffer uniformbuffer,uint32_t Size){
            VkDescriptorBufferInfo info{};
            info.buffer = uniformbuffer;
            info.offset =0;
            info.range = Size;



            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.dstSet = m_DescriptorSet;
            write.dstArrayElement = 0;
            write.dstBinding = 0;
            write.descriptorCount = 1;
            write.pBufferInfo = &info;
            
          //  write.pBufferInfo
          vkUpdateDescriptorSets(device,1,&write,0,nullptr);
    }
