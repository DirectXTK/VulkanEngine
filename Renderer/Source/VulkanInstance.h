#pragma once
#include "RendCore.h"
#include "SwapChain.h"
struct InstanceDesc {
	uint32_t ApiVersion{ VK_API_VERSION_1_0 };
	std::vector<const char*> AdditionalExtensions{};
	std::vector<const char*> LayerNames{};
	bool ValidationLayersEnabled{ false };
};
class VulkanInstance
{
public:

	static VkInstance CreateInstance(InstanceDesc& desc,VkDebugUtilsMessengerEXT* out_messenger);
	static void GetPhysicalDevice(VkInstance instance,VkSurfaceKHR surface,VkPhysicalDevice* pDevice);
	static VkDevice CreateLogicalDevice();

	static QueueFamilies GetQueueFamilies(VkPhysicalDevice device,VkSurfaceKHR surface);
private:
	static bool IsExtensionsSupported(std::vector<const char*>& extension);
	static void InitializeValidationLayers(VkInstance instance, VkDebugUtilsMessengerEXT* messenger);

	static bool IsPhysicalDeviceSiutable(VkPhysicalDevice device,VkSurfaceKHR surface);
	static bool CheckPhysicalDeviceSupportReqExtensions(VkPhysicalDevice device);

};

