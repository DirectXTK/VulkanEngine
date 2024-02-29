#pragma once
#include "RendCore.h"
struct ContextData
{
	VkDevice Device{};
	VkPhysicalDevice PDevice{};

	QueueFamilies QueueFamil{};
	VkQueue GraphicsQueue{};
	VkCommandPool CommandPool{};

	//Temp
	VkCommandBuffer TransferCommandBuffer{};
};
typedef ContextData* Context;

