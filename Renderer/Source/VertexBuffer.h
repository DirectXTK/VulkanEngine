#pragma once
#include "RendCore.h"
class VertexBuffer{
    public:
        VertexBuffer(VkDevice device,VkPhysicalDevice physdevice,uint64_t SizeBytes);

        VkBuffer* GetBuffer(){return &m_Buffer;}
        void UploadToBuffer(VkDevice device,void* data,uint32_t Size);
    private:
        uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicaldevice,uint32_t Allowedtypes,VkMemoryPropertyFlags flags);
    VkBuffer m_Buffer{};
    VkDeviceMemory m_BufferMemory{};
    uint64_t m_BufferSize{};
};