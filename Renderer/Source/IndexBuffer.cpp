#include "IndexBuffer.h"

        void IndexBuffer::Init(VkDevice device , VkPhysicalDevice physDevice,uint32_t SizeInBytes){
                VkBufferCreateInfo createinfo{};
                createinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                createinfo.size = SizeInBytes;
                createinfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                createinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                m_BufferSize = SizeInBytes;

                VkResult result = vkCreateBuffer(device,&createinfo,nullptr,&m_IndexBuffer);
                if(result != VK_SUCCESS)
                    Core::Log(ErrorType::Error,"Failed to create index buffer.");

                VkMemoryRequirements memreq{};

                vkGetBufferMemoryRequirements(device,m_IndexBuffer,&memreq);    

                VkMemoryAllocateInfo allocinfo{};
                allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocinfo.allocationSize = memreq.size;
                allocinfo.memoryTypeIndex = Core::FindMemoryTypeIndex(physDevice,memreq.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                result = vkAllocateMemory(device,&allocinfo,nullptr,&m_DeviceMemory);
                if(result != VK_SUCCESS)
                    Core::Log(ErrorType::Error,"Failed to allocate memory for index buffer.");

                vkBindBufferMemory(device,m_IndexBuffer,m_DeviceMemory,0);

        }
        void IndexBuffer::UploadToBuffer(VkDevice device,void* data,uint32_t SizeBytes){
                void *map{};
                vkMapMemory(device,m_DeviceMemory,0,m_BufferSize,0,&map);
                memcpy(map,data,SizeBytes);
                vkUnmapMemory(device,m_DeviceMemory);

              

        }
        IndexBuffer::~IndexBuffer(){



        }

