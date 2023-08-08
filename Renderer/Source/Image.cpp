#include "Image.h"
    Image::Image(VkPhysicalDevice pdevice,VkDevice device,VkFormat format,VkSharingMode sharemode,VkImageUsageFlags usage,VkMemoryPropertyFlags propflags,VkImageTiling tilling,uint32_t Width, uint32_t Height,VkImageLayout initformat ){
       m_Device = device;
       m_PDevice = pdevice; 
         
        VkImageCreateInfo imagecreateinfo{};
        imagecreateinfo.sType =VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imagecreateinfo.format = format;
        imagecreateinfo.sharingMode = sharemode;
        imagecreateinfo.imageType=VK_IMAGE_TYPE_2D;
        imagecreateinfo.initialLayout=initformat;
        imagecreateinfo.mipLevels=1;
        imagecreateinfo.tiling=tilling;
        imagecreateinfo.usage =usage;
        imagecreateinfo.arrayLayers=1;
        imagecreateinfo.extent.width = Width;
        imagecreateinfo.extent.height = Height;
        imagecreateinfo.extent.depth = 1;

        imagecreateinfo.samples = VK_SAMPLE_COUNT_1_BIT;

        m_Width =Width;
        m_Height = Height;

        VkResult result = vkCreateImage(device,&imagecreateinfo,nullptr,&m_Image);
        if(result != VK_SUCCESS)
          Core::Log(ErrorType::Error,"Failed to create image.");
        
        VkMemoryRequirements memreq{};
        vkGetImageMemoryRequirements(device,m_Image,&memreq);
      //  Core::Log(ErrorType::Info,"MemReq",propflags);
        VkMemoryAllocateInfo allocinfo{};
        allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocinfo.allocationSize = memreq.size;
        allocinfo.memoryTypeIndex=Core::FindMemoryTypeIndex(pdevice,memreq.memoryTypeBits,propflags);
        result = vkAllocateMemory(device,&allocinfo,nullptr,&m_Memory);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to allocate memory.");
        
        result =vkBindImageMemory(device,m_Image,m_Memory,0);
        if(result != VK_SUCCESS)
          Core::Log(ErrorType::Error,"Failed to bind memory to image.");

          m_ImageSize = memreq.size;
          CreateView(format,VK_IMAGE_ASPECT_COLOR_BIT,device);
    }
    VkImageView Image::CreateView(VkFormat format,VkImageAspectFlags aspectflags ,VkDevice device){

            VkImageViewCreateInfo createinfo{};
            createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createinfo.format = format;
            createinfo.image = m_Image;
            createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createinfo.subresourceRange.aspectMask =aspectflags;
            createinfo.subresourceRange.baseMipLevel=0;
            createinfo.subresourceRange.levelCount=1;
            createinfo.subresourceRange.layerCount=1;
            createinfo.subresourceRange.baseArrayLayer=0;
            


            VkResult result = vkCreateImageView(device,&createinfo,nullptr,&m_View);
            if(result != VK_SUCCESS)
              {
             Core::Log(ErrorType::Error,"Failed to create image view.");
            return m_View;
              
              }  
                

            return m_View;

    }
    Float2 Image::ReadPixel(uint32_t x, uint32_t y){
      void* map{};
        Float2* imagedata{};
        Float2 Ret{};
      
        imagedata = new Float2[m_Width*m_Height];
          Core::Log(ErrorType::Error,"With and height",m_Width,m_Height);

        VkDeviceSize offset{0};
      
        vkMapMemory(m_Device,m_Memory,offset,m_ImageSize,0,&map);
        memcpy(imagedata,map,m_ImageSize);
        vkUnmapMemory(m_Device,m_Memory);


        Ret=imagedata[x+(y*m_Width)];
      return Ret;
    }
