#include "Device.h"
#include "VulkanInstance.h"
VkDevice LogicalDevice::CreateLogicalDevice(VkPhysicalDevice physicalDevice,QueueFamilies families)
{
    VkDevice Device{};
    std::vector<VkDeviceQueueCreateInfo> queueInfos{};
    std::set<int> QueueFamilyIndices{families.Graphics, families.Presentation};
    std::vector<float> Priorities{};

    for (int Index : QueueFamilyIndices) {
        VkDeviceQueueCreateInfo  queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = Index;
        queueInfo.queueCount = 1;

         Priorities.push_back(1.0f);
        queueInfos.push_back(queueInfo);
    }
    for(uint32_t i=0;i < Priorities.size();i++)
        queueInfos[i].pQueuePriorities = &Priorities[i];

        
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.independentBlend = true;
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.fillModeNonSolid = true;

    VkPhysicalDeviceFeatures isSupported{};
    vkGetPhysicalDeviceFeatures(physicalDevice,&isSupported);
    VkBool32 wantedFeatures[sizeof(VkPhysicalDeviceFeatures)/4];
    VkBool32 supported[sizeof(VkPhysicalDeviceFeatures)/4];
    memcpy(wantedFeatures,&deviceFeatures,sizeof(VkPhysicalDeviceFeatures));
    memcpy(supported,&isSupported,sizeof(VkPhysicalDeviceFeatures));
    for(uint32_t i=0 ;i < sizeof(VkPhysicalDeviceFeatures)/4;i++){
        if(wantedFeatures[i] == VK_TRUE && supported[i]== VK_TRUE)
         wantedFeatures[i] = VK_TRUE;
         else 
         wantedFeatures[i] = VK_FALSE;
    }
    memcpy(&deviceFeatures,wantedFeatures,sizeof(VkPhysicalDeviceFeatures));

    VkPhysicalDeviceDescriptorIndexingFeatures indexFeatures{};
    indexFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexFeatures.shaderSampledImageArrayNonUniformIndexing = true;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &indexFeatures;
    deviceFeatures2.features = deviceFeatures;
    //check if supported
    vkGetPhysicalDeviceFeatures2(physicalDevice,&deviceFeatures2);

  

    VkDeviceCreateInfo devicecreateinfo{};
    devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devicecreateinfo.queueCreateInfoCount = (uint32_t)queueInfos.size();
    devicecreateinfo.pQueueCreateInfos = queueInfos.data();
    devicecreateinfo.enabledLayerCount = 0;
    devicecreateinfo.ppEnabledExtensionNames = nullptr;
    devicecreateinfo.enabledExtensionCount = (uint32_t)g_DeviceExtensions.size();
    devicecreateinfo.ppEnabledExtensionNames = g_DeviceExtensions.data();
    devicecreateinfo.pEnabledFeatures = nullptr;
    devicecreateinfo.pNext = &deviceFeatures2;




  vkCreateDevice(physicalDevice, &devicecreateinfo, nullptr, &Device);

    //if (result != VK_SUCCESS)
     //   Core::Log(ErrorType::Error, "Failed to create Device.");


    return Device;
 
}

void LogicalDevice::GetQueues(VkDevice device,QueueFamilies families, VkQueue* GraphicsQueue, VkQueue* PresentationQueue)
{
    vkGetDeviceQueue(device, families.Graphics, 0, GraphicsQueue);
    vkGetDeviceQueue(device, families.Presentation, 0, PresentationQueue);
}
