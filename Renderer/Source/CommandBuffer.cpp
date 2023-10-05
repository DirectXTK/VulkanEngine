#include "CommandBuffer.h"

VkCommandPool CommandBuffer::CreatCommandPool(VkDevice Device,uint32_t QueueIndex,VkCommandPoolCreateFlags flags)
{
    VkCommandPool CommandPool{};
    VkCommandPoolCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createinfo.queueFamilyIndex = QueueIndex;
    createinfo.flags = flags;

    VkResult result = vkCreateCommandPool(Device,&createinfo, nullptr, &CommandPool);
    if (result)
        Core::Log(ErrorType::Error, "Failed to create command pool");

    return CommandPool;
}
