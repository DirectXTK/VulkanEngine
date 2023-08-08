#include "Buffer.h"

        Buffer::Buffer(VkDevice device,VkBufferUsageFlags usage,VkSharingMode sharingmode,VkPhysicalDevice physdevice,VkMemoryPropertyFlags memoryflags,uint64_t SizeBytes){

            m_Device = device;

            VkBufferCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createinfo.size = SizeBytes;
            createinfo.sharingMode = sharingmode;
            createinfo.usage = usage;

            VkResult result = vkCreateBuffer(device,&createinfo,nullptr,&m_Buffer);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create buffer.");
            m_BufferSize= SizeBytes;

            VkMemoryRequirements memReq{};
            vkGetBufferMemoryRequirements(device,m_Buffer,&memReq);

            VkMemoryAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocinfo.allocationSize =memReq.size;
            allocinfo.memoryTypeIndex =Core::FindMemoryTypeIndex(physdevice,memReq.memoryTypeBits,memoryflags);


            result = vkAllocateMemory(device,&allocinfo,nullptr,&m_Memory);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to allocate memory for vertex buffer.");

            vkBindBufferMemory(device,m_Buffer,m_Memory,0);

        }
       // void Buffer::UploadToBuffer(VkDevice device,PhysicalDevice pdevice,void* data,uint32_t Size){
           // Buffer buffer(device,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_SHARING_MODE_EXCLUSIVE,pdevice,Size);
        //
        Float2 Buffer::ReadPixel(uint32_t x, uint32_t y,uint32_t Width,uint32_t Height){
       void* map{};
        Float2* imagedata{};
        Float2 Ret{};
      
        imagedata = new Float2[Width*Height];

        VkDeviceSize offset{0};
      
        vkMapMemory(m_Device,m_Memory,offset,m_BufferSize,0,&map);
        memcpy(imagedata,map,Width*Height*8);
        vkUnmapMemory(m_Device,m_Memory);

        //Core::Log(ErrorType::Error,"BufferSize ",m_BufferSize);
        //Core::Log(ErrorType::Error,"ImageDatSize ",Width*Height*8);
        
        Ret = imagedata[x + (y * Width)];
    

      //  Core::Log(ErrorType::Error,"First",imagedata[0].x," ",imagedata[0].y);
        delete[] imagedata;
      return Ret;
    }