#include "CommandPool.h"

        void CommandPool::Init(VkDevice device,uint32_t QueueIndex){



            VkCommandPoolCreateInfo poolinfo{};
            poolinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolinfo.queueFamilyIndex = QueueIndex;
            poolinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            VkResult result = vkCreateCommandPool(device,&poolinfo,nullptr,&m_CommandPool);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create command pool.");


        }
