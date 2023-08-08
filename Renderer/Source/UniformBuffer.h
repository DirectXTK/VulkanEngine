#pragma once
#include "RendCore.h"
class UniformBuffer{
    public:
        void Init(VkDevice device,VkPhysicalDevice physdevice,uint64_t SizeBytes);

        

        VkBuffer* GetBuffer(){return &m_Buffer;}
        void UploadToBuffer(VkDevice device,void* data,uint32_t Size);
    private:
        VkBuffer m_Buffer{};
        VkDeviceMemory m_BufferMemory{};
     uint64_t m_BufferSize{};
};