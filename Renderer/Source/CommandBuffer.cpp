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

VkCommandBuffer CommandBuffer::CreateCommandBuffer(Context context,VkCommandPool pool)
{
    VkCommandBuffer outcommandbuffer{};

    VkCommandBufferAllocateInfo allocinfo{};
    allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocinfo.commandPool = pool;
    allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocinfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(context->Device, &allocinfo,&outcommandbuffer);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create command buffers.");



    VkCommandBufferAllocateInfo allocinfo2{};
    allocinfo2.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocinfo2.commandPool = pool;
    allocinfo2.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocinfo2.commandBufferCount = 1;

    result = vkAllocateCommandBuffers(context->Device, &allocinfo2, &outcommandbuffer);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create command buffers.");

    return outcommandbuffer;
}

VkCommandBuffer CommandBuffer::StartSingleUseCommandBuffer(Context context, VkCommandPool pool)
{
    VkCommandBuffer outcommandbuffer = CreateCommandBuffer(context,pool);

    VkCommandBufferBeginInfo begininfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begininfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(outcommandbuffer, &begininfo);

    return outcommandbuffer;
}

void CommandBuffer::EndSingleUseCommandBuffer(Context context,VkCommandPool pool, VkCommandBuffer commandBuffer)
{
    VkResult result;
    VkFenceCreateInfo FenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

    FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkFence fence{};
    result =vkCreateFence(context->Device, &FenceCreateInfo, nullptr, &fence);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create fence.");
    result =vkResetFences(context->Device, 1, &fence);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to reset fence.");


    VkSubmitInfo SubmitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &commandBuffer;


    vkEndCommandBuffer(commandBuffer);
    vkQueueSubmit(context->GraphicsQueue, 1, &SubmitInfo, fence);

   result =vkWaitForFences(context->Device, 1, &fence, true, 1*1000*1000*10);
   if (result != VK_SUCCESS)
       Core::Log(ErrorType::Error, "Failed to wait for fences.");
    vkFreeCommandBuffers(context->Device, pool, 1, &commandBuffer);
}
