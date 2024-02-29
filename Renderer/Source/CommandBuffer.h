#pragma once
#include "RendCore.h"
#include "Context.h"
class CommandBuffer
{
public:
	static VkCommandPool CreatCommandPool(VkDevice Device, uint32_t QueueIndex, VkCommandPoolCreateFlags flags);
	static VkCommandBuffer CreateCommandBuffer(Context context,VkCommandPool pool);

	static VkCommandBuffer StartSingleUseCommandBuffer(Context context, VkCommandPool pool);
	static void EndSingleUseCommandBuffer(Context context,VkCommandPool pool,VkCommandBuffer commandBuffer);
};

