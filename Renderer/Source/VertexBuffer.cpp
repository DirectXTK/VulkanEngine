#include "VertexBuffer.h"

        VertexBuffer::VertexBuffer(VkDevice device,VkPhysicalDevice physdevice,uint64_t SizeBytes){

            VkBufferCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            createinfo.size = SizeBytes;
            createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createinfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            VkResult result = vkCreateBuffer(device,&createinfo,nullptr,&m_Buffer);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create buffer.");
            m_BufferSize= SizeBytes;

            VkMemoryRequirements memReq{};
            vkGetBufferMemoryRequirements(device,m_Buffer,&memReq);

            VkMemoryAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocinfo.allocationSize =memReq.size;
            allocinfo.memoryTypeIndex =FindMemoryTypeIndex(physdevice,memReq.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);


            result = vkAllocateMemory(device,&allocinfo,nullptr,&m_BufferMemory);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to allocate memory for vertex buffer.");

            vkBindBufferMemory(device,m_Buffer,m_BufferMemory,0);

        }
        void VertexBuffer::UploadToBuffer(VkDevice device,void* data,uint32_t Size){
            void * map{};
            vkMapMemory(device,m_BufferMemory,0,m_BufferSize,0,&map);
            memcpy(map,data,Size);
            vkUnmapMemory(device,m_BufferMemory);
        }

        uint32_t VertexBuffer::FindMemoryTypeIndex(VkPhysicalDevice physicaldevice,uint32_t Allowedtypes,VkMemoryPropertyFlags flags){
            VkPhysicalDeviceMemoryProperties memprops{};
            vkGetPhysicalDeviceMemoryProperties(physicaldevice,&memprops);

            for(int i =0;i < memprops.memoryTypeCount;i++){

                if((Allowedtypes&(1<<i))){
                    if((memprops.memoryTypes[i].propertyFlags & flags)==flags){
                        return i;
                    }

                }
            }
        }


