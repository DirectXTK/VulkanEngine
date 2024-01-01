#pragma once
#include "RendCore.h"

class IndexBuffer{
    public:
        void Init(VkDevice device , VkPhysicalDevice physDevice,uint32_t SizeInBytes);

        void UploadToBuffer(VkDevice device,void* data,uint32_t SizeBytes);

        VkBuffer* GetBuffer(){return &m_IndexBuffer;}
        
        ~IndexBuffer();
    private:
    VkBuffer m_IndexBuffer{};
    uint32_t m_BufferSize{};
    VkDeviceMemory m_DeviceMemory{};
};