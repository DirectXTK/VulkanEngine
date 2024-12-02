#pragma once
#include "RendCore.h"
#include "Context.h"
#include "Buffer.h"
class Image{
    public:
    Image(VkPhysicalDevice pdevice,VkDevice device,VkFormat format,VkSharingMode sharemode,VkImageUsageFlags usage,VkMemoryPropertyFlags propflags,VkImageTiling tilling,uint32_t Width, uint32_t Height,VkImageLayout initformat = VK_IMAGE_LAYOUT_UNDEFINED,VkImageAspectFlags ImageAspect = VK_IMAGE_ASPECT_COLOR_BIT);
    Image(std::string Path);
    Image() {}


    VkImageView CreateView(VkFormat format,VkImageAspectFlags aspectflags ,VkDevice device);
    VkImage GetImage(){return m_Image;}
    VkImageView GetImageView(){return m_View;}
    uint64_t GetSize(){return m_ImageSize;}
    VkExtent2D GetExtent(){return {(uint32_t)m_Width,(uint32_t)m_Height};}

    void CopyDataFromBuffer(Context context, Buffer* buffer, VkCommandBuffer commandbuffer);

    void SetImage(VkImage image){m_Image = image;}
    //Uploads the latest data.
    void UploadImageData();
    Float4 ReadPixel(uint32_t x, uint32_t y);

    private:
            VkImage m_Image{};
            VkImageView m_View{};
            VkDeviceMemory m_Memory{};
            VkDevice m_Device{};
            VkPhysicalDevice m_PDevice{};
            uint32_t m_Width{}, m_Height{};
            Float4* m_ImageData{};
            uint32_t m_ChannelCount{}; 
          uint64_t m_ImageSize{};

};