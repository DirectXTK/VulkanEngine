#pragma once
#include "RendCore.h"
class CommandBuffer
{
public:
	static VkCommandPool CreatCommandPool(VkDevice Device, uint32_t QueueIndex, VkCommandPoolCreateFlags flags);
	static VkCommandBuffer CreateCommandBuffer(VkCommandPool pool);

};

