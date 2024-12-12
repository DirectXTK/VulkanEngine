#include "Texture.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
Texture::Texture(Context context,uint32_t Width, uint32_t Height,uint32_t ChannelCount,uint32_t* Pixels)
{
	m_TextureData = new TextureData();
	m_TextureData->m_Context = context;
	m_TextureData->m_Width = Width;
	m_TextureData->m_Height = Height;
	m_TextureData->m_ChannelCount = ChannelCount;
	CreateTexture(Pixels);
	m_TextureData->m_TextureAtlasData = new TextureAtlasCoords();
	m_TextureData->m_TextureCount = 1;

	m_TextureData->m_TextureAtlasData->Points[0] = { 0.0f,0.25f };
	m_TextureData->m_TextureAtlasData->Points[1] = { 0.0f,0.0f };
	m_TextureData->m_TextureAtlasData->Points[2] = { 0.25f,0.0f };
	m_TextureData->m_TextureAtlasData->Points[3] = { 0.25f,0.25f };
}

Texture::Texture(Context context, std::string Path)
{
	m_TextureData = new TextureData();
	m_TextureData->m_Context = context;
	std::string Extension= Core::GetFileExtension(Path);
	//Loads texture normaly
	if (Extension == "png") {
		unsigned char* InitData = LoadTextureDataFromFile(Path);
		if (InitData) {

			CreateTexture(InitData);
			m_TextureData->m_TextureAtlasData = new TextureAtlasCoords();
			m_TextureData->m_TextureCount = 1;

			m_TextureData->m_TextureAtlasData->Points[0] = { 0.0f,1.0f };
			m_TextureData->m_TextureAtlasData->Points[1] = { 0.0f,0.0f };
			m_TextureData->m_TextureAtlasData->Points[2] = { 1.0f,0.0f };
			m_TextureData->m_TextureAtlasData->Points[3] = { 1.0f,1.0f };
			delete[] InitData;
		}
		
	}//Loads texture as an atlas
	else if (Extension == "json") {
		std::string TexturePath = Path.substr(0, Path.find("."));
		TexturePath += ".png";
		unsigned char* InitData = LoadTextureDataFromFile(TexturePath);
		if (InitData) {

			CreateTexture(InitData);
			CreateTextureAtlas(Path);
		
			delete[] InitData;
		}
	}
}

Texture::Texture(Texture* texture, uint32_t TextureIndex)
{
	m_TextureData = texture->m_TextureData;
	m_TextureIndex = TextureIndex;
}



void Texture::TrasitionFormat(VkImageLayout OldLayout, VkImageLayout NewLayout,VkCommandBuffer CommandBuffer)
{
	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.oldLayout = OldLayout;
	barrier.newLayout = NewLayout;
	barrier.image = m_TextureData->m_Image;
	barrier.dstQueueFamilyIndex = m_TextureData->m_Context->QueueFamil.Graphics;
	barrier.srcQueueFamilyIndex = m_TextureData->m_Context->QueueFamil.Graphics;
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



	vkCmdCopyBufferToImage(commandbuffer, *srcbuffer->GetBuffer(), m_TextureData->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


}

Texture** Texture::CreateTextureAtlases(uint32_t* out_AtlasCount)
{
	Texture** textures = new Texture*[m_TextureData->m_TextureCount];
	*out_AtlasCount = m_TextureData->m_TextureCount;
	for (uint32_t i = 0; i < m_TextureData->m_TextureCount; i++) {
		textures[i] = new Texture(this, i);
	}
	return textures;
}

Texture::~Texture()
{
	delete m_TextureData;
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
	imagecreateinfo.extent.width = m_TextureData->m_Width;
	imagecreateinfo.extent.height = m_TextureData->m_Height;
	imagecreateinfo.extent.depth = 1;

	imagecreateinfo.samples = VK_SAMPLE_COUNT_1_BIT;


	VkResult result = vkCreateImage(m_TextureData->m_Context->Device, &imagecreateinfo, nullptr, &m_TextureData->m_Image);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to create image.");

	VkMemoryRequirements memreq{};
	vkGetImageMemoryRequirements(m_TextureData->m_Context->Device, m_TextureData->m_Image, &memreq);
	//  Core::Log(ErrorType::Info,"MemReq",propflags);
	VkMemoryAllocateInfo allocinfo{};
	allocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocinfo.allocationSize = memreq.size;
	allocinfo.memoryTypeIndex = Core::FindMemoryTypeIndex(m_TextureData->m_Context->PDevice, memreq.memoryTypeBits, MemoryPropertyFlags);
	result = vkAllocateMemory(m_TextureData->m_Context->Device, &allocinfo, nullptr, &m_TextureData->m_DeviceMemory);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to allocate memory.");

	result = vkBindImageMemory(m_TextureData->m_Context->Device, m_TextureData->m_Image, m_TextureData->m_DeviceMemory, 0);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to bind memory to image.");

	m_TextureData->m_DeviceSize = memreq.size;
	m_TextureData->m_ImageView = CreateView(Format, VK_IMAGE_ASPECT_COLOR_BIT, m_TextureData->m_Context->Device);
}

VkImageView Texture::CreateView(VkFormat Format, VkImageAspectFlagBits AspectFlags, VkDevice Device)
{
	VkImageViewCreateInfo createinfo{};
	createinfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createinfo.format = Format;
	createinfo.image = m_TextureData->m_Image;
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



	VkResult result = vkCreateImageView(Device, &createinfo, nullptr, &m_TextureData->m_ImageView);
	if (result != VK_SUCCESS)
	{
		Core::Log(ErrorType::Error, "Failed to create image view.");
		return m_TextureData->m_ImageView;

	}


	return m_TextureData->m_ImageView;
}

unsigned char* Texture::LoadTextureDataFromFile(std::string Path)
{
	std::string RealPath = Path;
	stbi_uc* image = stbi_load(RealPath.c_str(), (int*)&m_TextureData->m_Width, (int*)&m_TextureData->m_Height, (int*)&m_TextureData->m_ChannelCount, STBI_rgb_alpha);
	if (!image)
	{
		Core::Log(ErrorType::Error, "Failed to load texture file ", RealPath);
		return nullptr;
	}
	return (unsigned char*)image;
}

void Texture::CreateTexture(void* initData)
{
	VkDeviceSize texturesize = m_TextureData->m_Width * m_TextureData->m_Height * m_TextureData->m_ChannelCount;
	VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

	//create stagging buffer
	BufferDesc bufferdesc{};
	bufferdesc.Device = m_TextureData->m_Context->Device;
	bufferdesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferdesc.Physdevice = m_TextureData->m_Context->PDevice;
	bufferdesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
	bufferdesc.SizeBytes = texturesize;
	bufferdesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	Buffer stagging(bufferdesc);
	stagging.UploadToBuffer(m_TextureData->m_Context->Device, initData, texturesize);

	//Create texture and view

	CreateImageAndView(ImageFormat,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_LAYOUT_UNDEFINED);

	VkCommandBuffer TempCommandBuffer = CommandBuffer::StartSingleUseCommandBuffer(m_TextureData->m_Context, m_TextureData->m_Context->CommandPool);

	TrasitionFormat(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TempCommandBuffer);


	CopyDataFromBuffer(TempCommandBuffer, *stagging.GetBuffer(), m_TextureData->m_Image);


	TrasitionFormat(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, TempCommandBuffer);

	CommandBuffer::EndSingleUseCommandBuffer(m_TextureData->m_Context, m_TextureData->m_Context->CommandPool, TempCommandBuffer);

	//Create Sampler //TEMP
	VkSamplerCreateInfo samplercreateinfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplercreateinfo.minFilter = VK_FILTER_NEAREST;
	samplercreateinfo.magFilter = VK_FILTER_NEAREST;
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

	VkResult result = vkCreateSampler(m_TextureData->m_Context->Device, &samplercreateinfo, nullptr, &m_TextureData->m_Sampler);
	if (result != VK_SUCCESS)
		Core::Log(ErrorType::Error, "Failed to create texture sampler.");



}

void Texture::CreateTextureAtlas(const std::string& MetaData)
{

	uint64_t Offset{};
	std::vector<TextureAtlasCoords> AtlasCoords{};



	uint64_t TileLocX{};
	uint64_t TileLocY{};

	uint32_t TileSizeX{};
	uint32_t TileSizeY{};


	std::string Data{};
	std::string Member{};
	uint64_t DataSize{};
	std::ifstream input(MetaData);
	if (!input.is_open()) {
		Core::Log(ErrorType::Error, "Failed to create texture atlas invalid PathToMetaData.");
		return;
	}
	input.seekg(0, input.end);
	DataSize = input.tellg();
	Data.resize(DataSize);
	input.seekg(0, input.beg);
	input.read(Data.data(), DataSize);

	Offset = Data.find("\"frame\"", Offset);
	while (Offset != (uint64_t)-1) {
		TextureAtlasCoords atlas{};


		Offset = Data.find("x", Offset) + 4;
		TileLocX = std::stoi(Data.substr(Offset, Data.find(",") - Offset));
		Offset = Data.find("y", Offset) + 4;
		TileLocY = std::stoi(Data.substr(Offset, Data.find(",") - Offset));
		Offset = Data.find("w", Offset) + 4;
		TileSizeX = std::stoi(Data.substr(Offset, Data.find(",") - Offset));
		Offset = Data.find("h", Offset) + 4;
		TileSizeY = std::stoi(Data.substr(Offset, Data.find("}" - 1) - Offset));

		atlas.Points[0] = { (float)TileLocX / (float)m_TextureData->m_Width,1.0f - (float)TileLocY / (float)m_TextureData->m_Height };
		atlas.Points[1] = { (float)TileLocX / (float)m_TextureData->m_Width,1.0f - ((float)TileLocY + TileSizeY) / (float)m_TextureData->m_Height };
		atlas.Points[2] = { (float)(TileLocX + TileSizeX) / (float)m_TextureData->m_Width,1.0f - (float)(TileLocY + TileSizeY) / (float)m_TextureData->m_Height };
		atlas.Points[3] = { (float)(TileLocX + TileSizeX) / (float)m_TextureData->m_Width,1.0f - (float)TileLocY / (float)m_TextureData->m_Height };

		m_Width = TileSizeX;
		m_Height = TileSizeY;

		//m_Vertices[m_VertexPointer].TexCoords = { 0.0f,1.0f };
		//m_Vertices[m_VertexPointer + 1].TexCoords = { 0.0f,0.0f };
		//m_Vertices[m_VertexPointer + 2].TexCoords = { 1.0f,0.0f };
		//m_Vertices[m_VertexPointer + 3].TexCoords = { 1.0f,1.0f };
		AtlasCoords.push_back(atlas);
		Offset = Data.find("\"frame\"", Offset);

	}
	//



	m_TextureData->m_TextureAtlasData = new TextureAtlasCoords[AtlasCoords.size()];
	m_TextureData->m_TextureCount = (uint32_t)AtlasCoords.size();
	memcpy(m_TextureData->m_TextureAtlasData, AtlasCoords.data(), sizeof(TextureAtlasCoords) * AtlasCoords.size());


}

void Texture::CopyDataFromBuffer(VkCommandBuffer CommandBuffer,VkBuffer BufferSrc,VkImage ImageDst)
{
	VkBufferImageCopy region{  };
	region.bufferOffset = 0;
	region.bufferRowLength = m_TextureData->m_Width;
	region.bufferImageHeight = m_TextureData->m_Height;

	region.imageExtent.width = m_TextureData->m_Width;
	region.imageExtent.height = m_TextureData->m_Height;
	region.imageExtent.depth = 1;
	region.imageOffset = { 0,0,0 };

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 0;


	vkCmdCopyBufferToImage(CommandBuffer, BufferSrc, ImageDst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}
