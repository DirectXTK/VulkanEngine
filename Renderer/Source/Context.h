#pragma once
#include "RendCore.h"
#include "Buffer.h"
struct ContextData
{
	VkDevice Device{};
	VkPhysicalDevice PDevice{};

	QueueFamilies QueueFamil{};
	VkQueue GraphicsQueue{};
	VkCommandPool CommandPool{};
	
	std::vector<Buffer*>* m_TempBuffers;

	VkCommandBuffer CurrentCommandBuffer{};
	VkCommandBuffer TransferCommandBuffer{};

};
typedef ContextData* Context;

