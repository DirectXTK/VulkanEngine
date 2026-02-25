#include "SwapChain.h"
        SwapChain::SwapChain(GLFWwindow* window,VkInstance instance,Context context,VkSurfaceKHR surface){
            m_Context = context;
            m_Surface= surface;
            m_Instance = instance;
            m_Window = window;

        }

         void SwapChain::CreateSwapChain(uint32_t imageCount){
              m_Details = SwapChain::GetSwapChainCapabilities(m_Context->PDevice,m_Surface);
              VkExtent2D extent = ChooseSwapExtent();



              VkPresentModeKHR presentation =GetBestPresentationMode();

              VkSurfaceFormatKHR surfaceformat = GetBestSurfaceFormat();

              VkSwapchainCreateInfoKHR createinfo{};
             createinfo.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
             createinfo.imageFormat =surfaceformat.format;
             createinfo.surface = m_Surface;
             createinfo.imageColorSpace = surfaceformat.colorSpace;
             createinfo.presentMode = presentation;
             createinfo.imageExtent = extent;
             createinfo.minImageCount = imageCount;
             createinfo.imageArrayLayers =1;
             createinfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
             createinfo.preTransform = m_Details.Capabilities.currentTransform;
             createinfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
             createinfo.clipped = VK_TRUE;
              if(m_Context->QueueFamil.Graphics != m_Context->QueueFamil.Presentation)
              {
                uint32_t indices[] = { (uint32_t)m_Context->QueueFamil.Graphics,(uint32_t)m_Context->QueueFamil.Presentation};
                createinfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createinfo.queueFamilyIndexCount = 2;
                
                createinfo.pQueueFamilyIndices = indices;

              }else{
                  createinfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createinfo.queueFamilyIndexCount = 0;
                
                createinfo.pQueueFamilyIndices = nullptr;
              }




            VkResult result =vkCreateSwapchainKHR(m_Context->Device,&createinfo,nullptr,&m_SwapChain);
            if(result != VK_SUCCESS)
              Core::Log(ErrorType::Error,"Failed to create swap chain.",(int32_t)result);
        

            m_SwapChainFormat = surfaceformat.format;
            m_SwapChainExtent = extent;
            //Getting access to swap chain images

            uint32_t ImageCount{};
            std::vector<VkImage> Images{};
            TextureCreateInfo swapChainTextureInfo{};


            vkGetSwapchainImagesKHR(m_Context->Device,m_SwapChain,&ImageCount,nullptr);
            m_SwapChainImages.resize(ImageCount);
            Images.resize(ImageCount);
            vkGetSwapchainImagesKHR(m_Context->Device,m_SwapChain,&ImageCount,Images.data());
            for(uint32_t i =0;i < ImageCount;i++){

                m_SwapChainImages[i]= new Texture(m_Context,surfaceformat.format,Images[i],extent);
            
            }
            
           }
        void SwapChain::DestroyImageViews(){
          for(uint32_t i=0;i < m_SwapChainImages.size();i++){
            m_SwapChainImages[i]->RemoveImage();
            delete m_SwapChainImages[i];
          }
        }
       

        bool SwapChain::TransitionLayout(VkImage image,VkImageLayout imageLayout,VkCommandBuffer commandBuffer){
          VkPipelineStageFlagBits srcFlagBits{},dstFlagBits{};

          VkImageMemoryBarrier imagePresentBarrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
          imagePresentBarrier.image = image;
          imagePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
          imagePresentBarrier.subresourceRange.baseArrayLayer = 0;
          imagePresentBarrier.subresourceRange.layerCount=1;
          imagePresentBarrier.subresourceRange.levelCount = 1;
          imagePresentBarrier.subresourceRange.baseMipLevel =0;
          imagePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
          imagePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
          imagePresentBarrier.srcAccessMask =0;
          imagePresentBarrier.dstAccessMask =0;


          if(imageLayout == VK_IMAGE_LAYOUT_UNDEFINED){
            imagePresentBarrier.oldLayout = imageLayout;
            imagePresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            imagePresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            srcFlagBits = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstFlagBits = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;


          }else if(imageLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
          {
            imagePresentBarrier.oldLayout = imageLayout;
            imagePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            srcFlagBits = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dstFlagBits = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            imagePresentBarrier.dstAccessMask =VK_ACCESS_MEMORY_READ_BIT;

          }
          else{
            Core::Log(ErrorType::Error,"Invalid SwapChain TransitionFormat ",(int)imageLayout);
            return false;
          }


          vkCmdPipelineBarrier(commandBuffer,srcFlagBits,dstFlagBits,0,0,0,0,0,1,&imagePresentBarrier);
          return true;
        }

     void SwapChain::CreateSurface(){
     

    }
     SwapChainDetails SwapChain::GetSwapChainCapabilities(){
         return m_Details;
    }
       SwapChainDetails SwapChain::GetSwapChainCapabilities(VkPhysicalDevice device,VkSurfaceKHR surface){
                 SwapChainDetails swapchaindetails{};
            uint32_t SurfaceFormatCount{};
            uint32_t PresentationCount{};

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&swapchaindetails.Capabilities);


            vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&SurfaceFormatCount,nullptr);
            swapchaindetails.SurfaceFormats.resize(SurfaceFormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&SurfaceFormatCount,swapchaindetails.SurfaceFormats.data());


            vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&PresentationCount,nullptr);
            swapchaindetails.PresentModes.resize(PresentationCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&PresentationCount,swapchaindetails.PresentModes.data());


            return swapchaindetails;


       }
      bool SwapChain::IsSwapChainValid(SwapChainDetails details){

      return details.PresentModes.size() >0&&details.PresentModes.size()>0;
     }
     bool SwapChain::IsSwapChainValid(){

      return m_Details.PresentModes.size() >0&&m_Details.PresentModes.size()>0;
     }
     VkSurfaceFormatKHR SwapChain::GetBestSurfaceFormat(){


          if(m_Details.SurfaceFormats.size()==1&& m_Details.SurfaceFormats[0].format ==VK_FORMAT_UNDEFINED)
          {
            Core::Log(ErrorType::Info,"All formats are supported.");
            return {VK_FORMAT_R8G8B8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
          }

          for(int i=0;i < m_Details.SurfaceFormats.size();i++){
              if((m_Details.SurfaceFormats[i].format ==VK_FORMAT_R8G8B8A8_UNORM||m_Details.SurfaceFormats[i].format ==VK_FORMAT_B8G8R8A8_UNORM)&&m_Details.SurfaceFormats[i].colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return m_Details.SurfaceFormats[i];
          }

          return m_Details.SurfaceFormats[0];
     }
     VkPresentModeKHR SwapChain::GetBestPresentationMode(){

        for(int i=0;i <m_Details.PresentModes.size();i++){

            if(m_Details.PresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
              return m_Details.PresentModes[i];


        }
        return VK_PRESENT_MODE_FIFO_KHR;
     }
     VkExtent2D SwapChain::ChooseSwapExtent(){

        if(m_Details.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()){
            return m_Details.Capabilities.currentExtent;
        }
        int Width,Height{};
        VkExtent2D extent{};
        glfwGetFramebufferSize(m_Window,&Width,&Height);
         extent.width = Width;
        extent.height = Height;

        extent.width = std::max(std::min(m_Details.Capabilities.maxImageExtent.width,  extent.width ),m_Details.Capabilities.minImageExtent.width );
        extent.height = std::max(std::min(m_Details.Capabilities.maxImageExtent.height,  extent.height ),m_Details.Capabilities.minImageExtent.height );;
      return extent;
    }
    SwapChain::~SwapChain(){

      vkDestroySwapchainKHR(m_Context->Device,m_SwapChain,nullptr);
      for(uint32_t i=0;i<m_SwapChainImages.size();i++){
         m_SwapChainImages[i]->RemoveImage();
         delete m_SwapChainImages[i];
      }


    }
     



