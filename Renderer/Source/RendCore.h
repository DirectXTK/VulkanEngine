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

#define VULKANDEBUG(x,y) x== VK_SUCCESS? (void)0: Core::Log(ErrorType::Error,y,": ",(int64_t)x) 
#define MAX_FRAME_DRAWS 1


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
        //returns false when out of bounds.
        bool ReadPixel(Float2* pixelData,int32_t width,int32_t height,int32_t posx,int32_t posy,Float2* outReadPixel);
        uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicaldevice,uint32_t Allowedtypes,VkMemoryPropertyFlags flags);
        VkFormat ChooseBestFormat(VkPhysicalDevice pdevice,const std::vector<VkFormat>& formats,VkImageTiling tilling,VkFormatFeatureFlags flags);
        void CopyImageToImage(VkDevice device,VkPhysicalDevice pdevice,Image* src,Image* dst);
        Float2 ToNDC(const Float2& pos);
        Float2 ToScreenPixels(const Float2& pos);
        
        uint32_t GetTextureSlotCount(VkPhysicalDevice pdevice);
        bool IsFrustomCullable(const Float2& cameraPos,const Float2& cameraSize,float aspectRatio,const Float2& pos,const Float2& size);

        bool IsShaderExtension(const std::string& extension);
    }

