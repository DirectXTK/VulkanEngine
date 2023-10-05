#include "Buffer.h"

        Buffer::Buffer(BufferDesc desc){

            m_Device = desc.Device;

            VkBufferCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createinfo.size = desc.SizeBytes;
            createinfo.sharingMode = desc.Sharingmode;
            createinfo.usage = desc.Usage;

            VkResult result = vkCreateBuffer(desc.Device,&createinfo,nullptr,&m_Buffer);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create buffer.");
            m_BufferSize= desc.SizeBytes;

            VkMemoryRequirements memReq{};
            vkGetBufferMemoryRequirements(desc.Device,m_Buffer,&memReq);

            VkMemoryAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocinfo.allocationSize =memReq.size;
            allocinfo.memoryTypeIndex =Core::FindMemoryTypeIndex(desc.Physdevice,memReq.memoryTypeBits, desc.Memoryflags);

            result = vkAllocateMemory(desc.Device,&allocinfo,nullptr,&m_Memory);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to allocate memory for vertex buffer.");

            vkBindBufferMemory(desc.Device,m_Buffer,m_Memory,0);

        }
        void Buffer::UploadToBuffer(VkDevice device, void* data, uint64_t Size)
        {
            void* map{};
            vkMapMemory(device, m_Memory, 0, m_BufferSize, 0, &map);
            memcpy(map, data, Size);
            vkUnmapMemory(device, m_Memory);
        }
        Float2 Buffer::ReadPixel(uint32_t x, uint32_t y,uint32_t Width,uint32_t Height){
             void* map{};
             Float2* imagedata{};
             Float2 Ret{};
      
             imagedata = new Float2[Width*Height];

             VkDeviceSize offset{0};
           
             vkMapMemory(m_Device,m_Memory,offset,m_BufferSize,0,&map);
             memcpy(imagedata,map,Width*Height*8);
             vkUnmapMemory(m_Device,m_Memory);

             
             Ret = imagedata[x + (y * Width)];
    

             delete[] imagedata;
             return Ret;
    }