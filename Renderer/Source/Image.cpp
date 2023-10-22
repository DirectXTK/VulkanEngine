#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "RendCore.h"
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
    Image::Image(std::string Path)
    {
       // LoadTextureData(Path);


    }
    unsigned char* Image::LoadTextureData(std::string Path,int* Width,int* Height)
    {
        std::string RealPath = "Textures/" + Path;
        stbi_uc* image = stbi_load(RealPath.c_str(), &m_Width, &m_Height, &m_ChannelCount, STBI_rgb_alpha);

        if (!image)
        {
            Core::Log(ErrorType::Error, "Failed to load texture file ", RealPath);
            return nullptr;
        }
        m_ImageData = new Float4[m_Width * m_Height];
        for (int y = 0; y < m_Height; y++) {

            for (int x = 0; x < m_Width; x++) {
                m_ImageData[x + (y * m_Width)].r = image[(x * 4) + (y * m_Width * 4) + 0]/255.f;
                m_ImageData[x + (y * m_Width)].g = image[(x * 4) + (y * m_Width * 4) + 1]/255.f;
                m_ImageData[x + (y * m_Width)].b = image[(x * 4) + (y * m_Width * 4) + 2]/255.f;
                m_ImageData[x + (y * m_Width)].a = image[(x * 4) + (y * m_Width * 4) + 3]/255.f;


            }
        }
    
        return (unsigned char*)image;
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
    void Image::UploadImageData()
    {
        void* map{};
            m_ImageData = new Float4[m_Width * m_Height];
       


        

        VkDeviceSize offset{ 0 };

        vkMapMemory(m_Device, m_Memory, offset, m_ImageSize, 0, &map);
        memcpy(m_ImageData, map, m_ImageSize);
        vkUnmapMemory(m_Device, m_Memory);

    }
    Float4 Image::ReadPixel(uint32_t x, uint32_t y){
      return m_ImageData[x + (y * m_Width)];
    }
