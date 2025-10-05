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
    void UploadToBuffer(VkDevice device, void* data, uint64_t Size,uint64_t Offset=0);
    Float2 ReadPixel(uint32_t x, uint32_t y, uint32_t Width, uint32_t Height);
    //if size =0 when the whole buffer size is used.
    void LoadFromBufferToVar(void* out_data,uint64_t size=0,uint64_t Offset=0);
    BufferDesc GetBufferDesc() { return m_Desc; }

    ~Buffer();
private:
    VkDeviceMemory m_Memory{};
    VkBuffer m_Buffer{};
    uint64_t m_BufferSize{};
    VkDevice m_Device{};
    BufferDesc m_Desc{};

};