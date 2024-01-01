#pragma once
#include "Image.h"
class SwapChain{
    public:
        SwapChain(VkInstance instance ,VkPhysicalDevice pdevice,VkDevice device,VkSurfaceKHR surface,QueueFamilies queuesfamilies);

        VkSurfaceKHR GetSurface(){return m_Surface;}


        bool IsSwapChainValid();
        static bool IsSwapChainValid(SwapChainDetails details);


        SwapChainDetails GetSwapChainCapabilities();
        static SwapChainDetails GetSwapChainCapabilities(VkPhysicalDevice device,VkSurfaceKHR surface);

        VkSwapchainKHR GetSwapChain(){return m_SwapChain;}

        VkExtent2D GetExtent(){return m_SwapChainExtent;}
        VkFormat GetFormat(){return m_SwapChainFormat;}

        uint32_t GetSwapChainImageCount(){ return (uint32_t)m_SwapChainImages.size();}
        Image* GetSwapChainImage(uint32_t Index){ return &m_SwapChainImages[Index];}
        
         void CreateSwapChain();
         void ReCreateSwapChain();

         ~SwapChain();
    private:
         VkSurfaceFormatKHR GetBestSurfaceFormat();
         VkPresentModeKHR GetBestPresentationMode();
         VkExtent2D ChooseSwapExtent();
         void CreateSurface();
     VkSwapchainKHR m_SwapChain{};
     SwapChainDetails m_Details{};
     GLFWwindow* m_Window{};
     VkInstance m_Instance{};
     VkPhysicalDevice m_PDevice{};
     VkDevice m_Device{};
     VkSurfaceKHR m_Surface{};
     QueueFamilies m_QueueFamilies{};

     VkFormat m_SwapChainFormat{};
     VkExtent2D m_SwapChainExtent{};

     std::vector<Image> m_SwapChainImages{};
};