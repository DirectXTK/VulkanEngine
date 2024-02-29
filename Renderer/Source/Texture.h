#pragma once
#include "RendCore.h"
#include "Buffer.h"
#include "Context.h"
struct TextureCreateInfo {
	VkFormat Format{ VK_FORMAT_R8G8B8A8_UNORM };
	VkImageLayout ImageLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	VkSharingMode SharingMode{ VK_SHARING_MODE_EXCLUSIVE };
	VkImageTiling ImageTilling{ VK_IMAGE_TILING_OPTIMAL };
	VkImageUsageFlagBits ImageUsageFlags{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
	VkMemoryPropertyFlagBits MemoryPropertyFlags{};
};
class Texture
{
public:
	Texture(Context context,uint32_t Width, uint32_t Height,uint32_t ChannelCount,uint32_t* Pixels);
	Texture(Context context,std::string Path);

	void TrasitionFormat(VkImageLayout OldLayout, VkImageLayout NewLayout, VkCommandBuffer CommandBuffer);
	void CopyFromBuffer(VkDevice device, Buffer* srcbuffer, VkCommandBuffer commandbuffer);

	VkImageView GetImageView() { return m_ImageView; }
	VkImage GetImage() { return m_Image; }
	VkSampler GetSampler() { return m_Sampler; }

	~Texture();
private:

	VkImageView CreateView(VkFormat Format,VkImageAspectFlagBits AspectFlags,VkDevice Device);
	void CreateImageAndView(VkFormat Format, VkSharingMode ShareMode, VkImageTiling ImageTilling, VkImageUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags, VkImageLayout InitialImageLayout);
	unsigned char* LoadTextureDataFromFile(std::string Path);
	void CreateTexture(void* initData);

	void CopyDataFromBuffer(VkCommandBuffer CommandBuffer, VkBuffer BufferSrc, VkImage ImageDst);

	VkImage m_Image{};
	VkImageView m_ImageView{};
	uint32_t m_Width{};
	uint32_t m_Height{};
	uint32_t m_ChannelCount{};
	Context m_Context{};
	VkSampler m_Sampler{  };

	VkDeviceMemory m_DeviceMemory{};
	VkDeviceSize m_DeviceSize{};
};

