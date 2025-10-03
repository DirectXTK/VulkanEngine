#pragma once
#include "Texture.h"
class SwapChain{
    public:
        SwapChain(VkInstance instance ,Context context,VkSurfaceKHR surface);

        VkSurfaceKHR GetSurface(){return m_Surface;}


        bool IsSwapChainValid();
        static bool IsSwapChainValid(SwapChainDetails details);


        SwapChainDetails GetSwapChainCapabilities();
        static SwapChainDetails GetSwapChainCapabilities(VkPhysicalDevice device,VkSurfaceKHR surface);

        VkSwapchainKHR GetSwapChain(){return m_SwapChain;}

        VkExtent2D GetExtent(){return m_SwapChainExtent;}
        VkFormat GetFormat(){return m_SwapChainFormat;}

        uint32_t GetSwapChainImageCount(){ return (uint32_t)m_SwapChainImages.size();}
        Texture* GetSwapChainImage(uint32_t Index){ return m_SwapChainImages[Index];}
        
         void CreateSwapChain();

        void DestroyImageViews();
        bool TransitionLayout(VkImage image,VkImageLayout imageLayout,VkCommandBuffer commandBuffer);

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
     Context m_Context{};
     VkSurfaceKHR m_Surface{};

     VkFormat m_SwapChainFormat{};
     VkExtent2D m_SwapChainExtent{};

     std::vector<Texture*> m_SwapChainImages{};
};