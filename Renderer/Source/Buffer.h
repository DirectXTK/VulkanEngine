#pragma once
#include "RendCore.h"
struct BufferDesc {
    VkDevice Device{};
    VkBufferUsageFlags Usage{};
    VkSharingMode Sharingmode{};
    VkPhysicalDevice Physdevice{};
    VkMemoryPropertyFlags Memoryflags{};
    uint64_t SizeBytes{};
};
class Buffer {
public:
    Buffer(BufferDesc desc);

    VkBuffer* GetBuffer() { return &m_Buffer; }
    void UploadToBuffer(VkDevice device, void* data, uint64_t Size);
    Float2 ReadPixel(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height);
    BufferDesc GetBufferDesc() { return m_Desc; }

private:
    VkDeviceMemory m_Memory{};
    VkBuffer m_Buffer{};
    uint64_t m_BufferSize{};
    VkDevice m_Device{};
    BufferDesc m_Desc{};

};