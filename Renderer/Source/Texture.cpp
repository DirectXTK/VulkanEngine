#include "Texture.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
Texture::Texture(Context context,uint32_t Width, uint32_t Height,uint32_t ChannelCount,uint32_t* Pixels)
{
	/*m_Width = Width;
	m_Height = Height;
	m_ChannelCount = ChannelCount;
	CreateTexture(Pixels);

	Image imageloading;
	unsigned char* imagedata = imageloading.LoadTextureData(Path, (int*)&m_Width, (int*)&m_Height, (int*)&m_ChannelCount);


	m_Context = context;
	VkDeviceSize texturesize = m_Width * m_Height * m_ChannelCount;
	VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

	//create stagging buffer
	BufferDesc bufferdesc{};
	bufferdesc.Device = context->Device;
	bufferdesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferdesc.Physdevice = context->PDevice;
	bufferdesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
	bufferdesc.SizeBytes = texturesize;
	bufferdesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	Buffer stagging(bufferdesc);
	stagging.UploadToBuffer(context->Device, imagedata, texturesize);


	VkCommandBuffer TempCommandBuffer = CommandBuffer::StartSingleUseCommandBuffer(context, context->CommandPool);

	TrasitionFormat(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TempCommandBuffer);


	image.CopyDataFromBuffer(context, &stagging, TempCommandBuffer);


	TrasitionFormat(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, TempCommandBuffer);

	CommandBuffer::EndSingleUseCommandBuffer(context, context->CommandPool, TempCommandBuffer);

	//Create Sampler //TEMP
	VkSamplerCreateInfo samplercreateinfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplercreateinfo.minFilter = VK_FILTER_LINEAR;
	samplercreateinfo.magFilter = VK_FILTER_LINEAR;
	samplercreateinfo.anisotropyEnable = false;
	samplercreateinfo.maxAnisotropy = 0;
	samplercreateinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplercreateinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplercreateinfo.unnormalizedCoordinates = false;
	samplercreateinfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplercreateinfo.compareEnable = false;
	samplercreateinfo.mipLodBias = 0.0f;
	samplercreateinfo.maxLod = 0.0f;
	samplercreateinfo.minLod = 0.0f;

	VkResult result = vkCreateSampler(context->Device, &samplercreateinfo, nullptr, &m_Sampler);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to create texture sampler.");
		*/
}

Texture::Texture(Context context, std::string Path)
{
	m_Context = context;
	unsigned char* InitData = LoadTextureDataFromFile(Path);
	if (InitData) {

		CreateTexture(InitData);
		delete[] InitData;
	}
}



void Texture::TrasitionFormat(VkImageLayout OldLayout, VkImageLayout NewLayout,VkCommandBuffer CommandBuffer)
{
	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.oldLayout = OldLayout;
	barrier.newLayout = NewLayout;
	barrier.image = m_Image;
	barrier.dstQueueFamilyIndex = m_Context->QueueFamil.Graphics;
	barrier.srcQueueFamilyIndex = m_Context->QueueFamil.Graphics;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;

	VkPipelineStageFlags SourceStage{};
	VkPipelineStageFlagBits DstStage{};

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		DstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}

	vkCmdPipelineBarrier(CommandBuffer, SourceStage, DstStage, 0, 0, 0, 0, 0, 1, &barrier);


}

void Texture::CopyFromBuffer(VkDevice device, Buffer* srcbuffer, VkCommandBuffer commandbuffer)
{
	VkBufferImageCopy region{  };
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;

	region.imageExtent.width = m_Width;
	region.imageExtent.height = m_Height;
	region.imageExtent.depth = 1;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.baseArrayLayer = 0;



	vkCmdCopyBufferToImage(commandbuffer, *srcbuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


}

Texture::~Texture()
{
}

void Texture::CreateImageAndView(VkFormat Format,VkSharingMode ShareMode,VkImageTiling ImageTilling,VkImageUsageFlags UsageFlags,VkMemoryPropertyFlags MemoryPropertyFlags,VkImageLayout InitialImageLayout)
{
	VkImageCreateInfo imagecreateinfo{};
	imagecreateinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imagecreateinfo.format = Format;
	imagecreateinfo.sharingMode = ShareMode;
	imagecreateinfo.imageType = VK_IMAGE_TYPE_2D;
	imagecreateinfo.initialLayout = InitialImageLayout;
	imagecreateinfo.mipLevels = 1;
	imagecreateinfo.tiling = ImageTilling;
	imagecreateinfo.usage = UsageFlags;
	imagecreateinfo.arrayLayers = 1;
	imagecreateinfo.extent.width = m_Width;
	imagecreateinfo.extent.height = m_Height;
	imagecreateinfo.extent.depth = 1;

	imagecreateinfo.samples = VK_SAMPLE_COUNT_1_BIT;


	VkResult result = vkCreateImage(m_Context->Device, &imagecreateinfo, nullptr, &m_Image);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to create image.");

	VkMemoryRequirements memreq{};
	vkGetImageMemoryRequirements(m_Context->Device, m_Image, &memreq);
	//  Core::Log(ErrorType::Info,"MemReq",propflags);
	VkMemoryAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocinfo.allocationSize = memreq.size;
	allocinfo.memoryTypeIndex = Core::FindMemoryTypeIndex(m_Context->PDevice, memreq.memoryTypeBits, MemoryPropertyFlags);
	result = vkAllocateMemory(m_Context->Device, &allocinfo, nullptr, &m_DeviceMemory);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to allocate memory.");

	result = vkBindImageMemory(m_Context->Device, m_Image, m_DeviceMemory, 0);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to bind memory to image.");

	m_DeviceSize = memreq.size;
	m_ImageView = CreateView(Format, VK_IMAGE_ASPECT_COLOR_BIT, m_Context->Device);
}

VkImageView Texture::CreateView(VkFormat Format, VkImageAspectFlagBits AspectFlags, VkDevice Device)
{
	VkImageViewCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createinfo.format = Format;
	createinfo.image = m_Image;
	createinfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createinfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createinfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createinfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createinfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	createinfo.subresourceRange.aspectMask = AspectFlags;
	createinfo.subresourceRange.baseMipLevel = 0;
	createinfo.subresourceRange.levelCount = 1;
	createinfo.subresourceRange.layerCount = 1;
	createinfo.subresourceRange.baseArrayLayer = 0;



	VkResult result = vkCreateImageView(Device, &createinfo, nullptr, &m_ImageView);
	if (result != VK_SUCCESS)
	{
		Core::Log(ErrorType::Error, "Failed to create image view.");
		return m_ImageView;

	}


	return m_ImageView;
}

unsigned char* Texture::LoadTextureDataFromFile(std::string Path)
{
	std::string RealPath = Path;
	stbi_uc* image = stbi_load(RealPath.c_str(), (int*)&m_Width, (int*)&m_Height, (int*)&m_ChannelCount, STBI_rgb_alpha);
	if (!image)
	{
		Core::Log(ErrorType::Error, "Failed to load texture file ", RealPath);
		return nullptr;
	}
	return (unsigned char*)image;
}

void Texture::CreateTexture(void* initData)
{
	VkDeviceSize texturesize = m_Width * m_Height * m_ChannelCount;
	VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

	//create stagging buffer
	BufferDesc bufferdesc{};
	bufferdesc.Device = m_Context->Device;
	bufferdesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferdesc.Physdevice = m_Context->PDevice;
	bufferdesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
	bufferdesc.SizeBytes = texturesize;
	bufferdesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	Buffer stagging(bufferdesc);
	stagging.UploadToBuffer(m_Context->Device, initData, texturesize);

	//Create texture and view

	CreateImageAndView(VK_FORMAT_R8G8B8A8_UNORM,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_LAYOUT_UNDEFINED);

	VkCommandBuffer TempCommandBuffer = CommandBuffer::StartSingleUseCommandBuffer(m_Context, m_Context->CommandPool);

	TrasitionFormat(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TempCommandBuffer);


	CopyDataFromBuffer(TempCommandBuffer, *stagging.GetBuffer(), m_Image);


	TrasitionFormat(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, TempCommandBuffer);

	CommandBuffer::EndSingleUseCommandBuffer(m_Context, m_Context->CommandPool, TempCommandBuffer);

	//Create Sampler //TEMP
	VkSamplerCreateInfo samplercreateinfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplercreateinfo.minFilter = VK_FILTER_LINEAR;
	samplercreateinfo.magFilter = VK_FILTER_LINEAR;
	samplercreateinfo.anisotropyEnable = false;
	samplercreateinfo.maxAnisotropy = 0;
	samplercreateinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplercreateinfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplercreateinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplercreateinfo.unnormalizedCoordinates = false;
	samplercreateinfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplercreateinfo.compareEnable = false;
	samplercreateinfo.mipLodBias = 0.0f;
	samplercreateinfo.maxLod = 0.0f;
	samplercreateinfo.minLod = 0.0f;

	VkResult result = vkCreateSampler(m_Context->Device, &samplercreateinfo, nullptr, &m_Sampler);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to create texture sampler.");

}

void Texture::CopyDataFromBuffer(VkCommandBuffer CommandBuffer,VkBuffer BufferSrc,VkImage ImageDst)
{
	VkBufferImageCopy region{  };
	region.bufferOffset = 0;
	region.bufferRowLength = m_Width;
	region.bufferImageHeight = m_Height;

	region.imageExtent.width = m_Width;
	region.imageExtent.height = m_Height;
	region.imageExtent.depth = 1;
	region.imageOffset = { 0,0,0 };

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 0;


	vkCmdCopyBufferToImage(CommandBuffer, BufferSrc, ImageDst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}