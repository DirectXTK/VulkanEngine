#include "RendCore.h"
#include "Image.h"
namespace Core{
        uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicaldevice,uint32_t Allowedtypes,VkMemoryPropertyFlags flags){
            VkPhysicalDeviceMemoryProperties memprops{};
            vkGetPhysicalDeviceMemoryProperties(physicaldevice,&memprops);
           // Core::Log(ErrorType::Error,"Allowed types ",Allowedtypes);
             //Core::Log(ErrorType::Error,"Req",flags);
           // Core::Log(ErrorType::Error, "Allowed types", (Allowedtypes & (1 << 0)));
            for(uint32_t i =0;i < memprops.memoryTypeCount;i++){

                uint32_t MemoryTypeBits = (1 << i);
                bool IsReq = Allowedtypes & MemoryTypeBits;

                VkMemoryPropertyFlags properties = memprops.memoryTypes[i].propertyFlags;
                bool HasReq = (properties & flags) == flags;
                if (IsReq && HasReq)
                    return i;
            }
            Core::Log(ErrorType::Error,"Couldn't find memory type index.");
            return 0;
        }
        VkFormat ChooseBestFormat(VkPhysicalDevice pdevice,const std::vector<VkFormat>& formats,VkImageTiling tilling,VkFormatFeatureFlags flags){

                for(int i =0;i < formats.size();i++){

                    VkFormatProperties properties{};
                    vkGetPhysicalDeviceFormatProperties(pdevice,formats[i],&properties);
                    if(tilling ==VK_IMAGE_TILING_OPTIMAL &&(properties.optimalTilingFeatures&flags)==flags){
                        return formats[i];

                    } else if(tilling ==VK_IMAGE_TILING_LINEAR &&(properties.linearTilingFeatures&flags)==flags){
                        return formats[i];
                        
                    }



                }
            Core::Log(ErrorType::Error,"Failed to find supported format.");
            return VK_FORMAT_UNDEFINED;
        }
        void CopyImageToImage(VkDevice device,VkPhysicalDevice pdevice,Image* src,Image* dst){



            
        }

}