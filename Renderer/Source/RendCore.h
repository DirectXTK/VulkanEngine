#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "AppCore.h"
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <cstring>
#include <array>
#include <set>

class Image;
const std::vector<const char*> g_DeviceExtensions={VK_KHR_SWAPCHAIN_EXTENSION_NAME};
struct QueueFamilies{
    int Graphics{-1};
    int Presentation{-1};
    bool IsValid(){if(Graphics !=-1 && Presentation != -1)return true; return false;}

};
struct SwapChainDetails{
    VkSurfaceCapabilitiesKHR Capabilities{};
    std::vector<VkSurfaceFormatKHR> SurfaceFormats{};
    std::vector<VkPresentModeKHR> PresentModes{};
};

    namespace Core{
        uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicaldevice,uint32_t Allowedtypes,VkMemoryPropertyFlags flags);
        VkFormat ChooseBestFormat(VkPhysicalDevice pdevice,const std::vector<VkFormat>& formats,VkImageTiling tilling,VkFormatFeatureFlags flags);
        void CopyImageToImage(VkDevice device,VkPhysicalDevice pdevice,Image* src,Image* dst);
    }

