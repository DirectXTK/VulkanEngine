#include "Device.h"
#include "VulkanInstance.h"
VkDevice LogicalDevice::CreateLogicalDevice(VkPhysicalDevice physicalDevice,QueueFamilies families)
{
    VkDevice Device{};
    std::vector<VkDeviceQueueCreateInfo> queueInfos{};
    std::set<int> QueueFamilyIndices{families.Graphics, families.Presentation};

    for (int Index : QueueFamilyIndices) {
        VkDeviceQueueCreateInfo  queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = Index;
        queueInfo.queueCount = 1;

        float Priority = 1.0f;
        queueInfo.pQueuePriorities = &Priority;

        queueInfos.push_back(queueInfo);
    }



    VkDeviceCreateInfo devicecreateinfo{};
    devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devicecreateinfo.queueCreateInfoCount = (uint32_t)queueInfos.size();
    devicecreateinfo.pQueueCreateInfos = queueInfos.data();
    devicecreateinfo.enabledLayerCount = 0;
    devicecreateinfo.ppEnabledExtensionNames = nullptr;
    devicecreateinfo.enabledExtensionCount = (uint32_t)g_DeviceExtensions.size();
    devicecreateinfo.ppEnabledExtensionNames = g_DeviceExtensions.data();


    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.independentBlend = true;
    deviceFeatures.samplerAnisotropy = true;

    devicecreateinfo.pEnabledFeatures = &deviceFeatures;


    VkResult result = vkCreateDevice(physicalDevice, &devicecreateinfo, nullptr, &Device);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create Device.");

    return Device;
 
}

void LogicalDevice::GetQueues(VkDevice device,QueueFamilies families, VkQueue* GraphicsQueue, VkQueue* PresentationQueue)
{
    vkGetDeviceQueue(device, families.Graphics, 0, GraphicsQueue);
    vkGetDeviceQueue(device, families.Presentation, 0, PresentationQueue);
}
