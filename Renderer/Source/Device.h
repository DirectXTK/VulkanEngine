#pragma once
#include "RendCore.h"
class LogicalDevice {
public:

	static VkDevice CreateLogicalDevice(VkPhysicalDevice devicem,QueueFamilies families);
	static void GetQueues(VkDevice device,QueueFamilies families,VkQueue* GraphicsQueue,VkQueue* PresentationQueue);
private:

};