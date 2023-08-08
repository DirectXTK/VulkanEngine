#pragma once
#include "RendCore.h"
class Buffer{
    public:
          Buffer(VkDevice device,VkBufferUsageFlags usage,VkSharingMode sharingmode,VkPhysicalDevice physdevice,VkMemoryPropertyFlags memoryflags,uint64_t SizeBytes);

        VkBuffer* GetBuffer(){return &m_Buffer;}
        void UploadToBuffer(VkDevice device,void* data,uint32_t Size);
        Float2 ReadPixel(uint32_t x, uint32_t y,uint32_t Width,uint32_t Height);
    private:
    VkDeviceMemory m_Memory{};
    VkBuffer m_Buffer{};
    uint64_t m_BufferSize{};
    VkDevice m_Device{};

};