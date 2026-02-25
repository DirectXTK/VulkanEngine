#include "Texture.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#include "stb-master/stb_image_write.h"

uint32_t CalculateBytesPerPixel(VkFormat format){
		switch(format){
			case VK_FORMAT_R8G8B8A8_UNORM:
				return 4;
			case VK_FORMAT_R8G8B8A8_UINT:
				return 4;
			case VK_FORMAT_R8G8B8_UINT:
				return 3;
			case VK_FORMAT_R8G8_UNORM:
				return 2;
			case VK_FORMAT_R8G8_UINT:
				return 2;
			case VK_FORMAT_R8_UNORM:
				return 1;
			case VK_FORMAT_R8_UINT:
				return 1;
			case VK_FORMAT_R32G32_UINT:
				return 8;
			case VK_FORMAT_D24_UNORM_S8_UINT: 
				return 4;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				return 8;
			case VK_FORMAT_B8G8R8A8_UNORM:
				return 4;
			default :{
				Core::Log(ErrorType::Error,"CalculateBytesPerPixel invalid type.Type = ",(uint32_t)format);
			}
		}
		return 0;
}
	Texture::Texture(Context context, const TextureCreateInfo& createInfo,const TextureType& type){
		m_Context = context;
		m_TextureType = type;
		m_Width = createInfo.Width;
		m_Height = createInfo.Height;
		m_ChannelCount = CalculateBytesPerPixel(createInfo.Format);
		switch(type){
			case TextureType::Texture:{
			CreateTexture(createInfo.Format,createInfo.SharingMode,createInfo.ImageTilling,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,createInfo.MemoryPropertyFlags,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,createInfo.Pixels);
				break;
			}
			case TextureType::ColorAttachment:{
			CreateTexture(createInfo.Format,createInfo.SharingMode,createInfo.ImageTilling,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|createInfo.ImageUsageFlags,createInfo.MemoryPropertyFlags,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_UNDEFINED);
				break;
			}
			case TextureType::DepthStencilAttachment :{
			CreateTexture(createInfo.Format,createInfo.SharingMode,createInfo.ImageTilling,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,createInfo.MemoryPropertyFlags,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_UNDEFINED,nullptr,VkImageAspectFlagBits(VK_IMAGE_ASPECT_DEPTH_BIT|VK_IMAGE_ASPECT_STENCIL_BIT));
				break;
			}
			default :{
				Core::Log(ErrorType::Error,"Texture type doesn't exist.");
				break;
			}
		}
		

	}

	Texture::Texture(Context context,VkFormat format,VkImage image,VkExtent2D extent){
		m_Context = context;
		m_Image = image;
		m_TextureType = TextureType::SwapChainImage;
		m_AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		m_Width = extent.width;
		m_Height  = extent.height;
		m_ChannelCount = CalculateBytesPerPixel(format);
		CreateView(format,m_AspectMask,m_Context->Device);
	}
	Texture::Texture(Texture* texture, uint32_t TextureIndex){
		*this = *texture;
		m_TextureIndex= TextureIndex;

	}
Texture::Texture(Context context, const TextureCreateInfo& createInfo ,std::string Path,TextureType type)
{
	std::string Extension= Core::GetFileExtension(Path);
	m_TextureType = type;
	uint64_t Width{}, Height{};
	m_Context = context;
	//Loads texture normaly
	if (Extension == "png") {
		unsigned char* InitData = LoadTextureDataFromFile(Path,&Width,&Height);
		if (InitData) {
			m_Width = Width;
			m_Height =Height;


			CreateTexture(createInfo.Format,createInfo.SharingMode,createInfo.ImageTilling,createInfo.ImageUsageFlags,createInfo.MemoryPropertyFlags,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,InitData);

			free(InitData);
		}
		
	}//Loads texture as an atlas
	else if (Extension == "json") {
		
		std::string TexturePath = Path.substr(0, Path.find("."));
		TexturePath += ".png";
		unsigned char* InitData = LoadTextureDataFromFile(TexturePath,&Width,&Height);
		if (InitData) {
			//creates the parent and child textures.
			CreateTextureAtlasAndParent(Path,Width,Height);
		
			free(InitData);
		}
	}
}

bool Texture::WriteToFile(const std::string& path){


	BufferDesc desc{};
	desc.Device = m_Context->Device;
	desc.Memoryflags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	desc.Physdevice = m_Context->PDevice;
	desc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
	desc.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	desc.SizeBytes = m_ChannelCount*m_Width*m_Height;

	Buffer staggingBuffer(desc);
	VkCommandBuffer commandBuffer =  CommandBuffer::StartSingleUseCommandBuffer(m_Context,m_Context->CommandPool);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.imageExtent = {(uint32_t)m_Width,(uint32_t)m_Height,1};
	region.imageOffset ={0,0,0};
	region.imageSubresource.aspectMask = m_AspectMask;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 0;

	VkImageLayout oldLayout= m_Layout;

	TrasitionFormat(true,m_Layout,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,commandBuffer);
	vkCmdCopyImageToBuffer(commandBuffer,m_Image,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,*staggingBuffer.GetBuffer(),1,&region);
	TrasitionFormat(true,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,commandBuffer);


	CommandBuffer::EndSingleUseCommandBuffer(m_Context,m_Context->CommandPool,commandBuffer);

	char* data = new char[m_ChannelCount*m_Width*m_Height];
	staggingBuffer.LoadFromBufferToVar(data);
	int ret = stbi_write_png(path.c_str(),m_Width,m_Height,m_ChannelCount,data,4*m_Width);
	if(!ret)
		Core::Log(ErrorType::Error,"Failed to write to file this texture{Texture::WriteToFile} ",m_Width,m_Height,m_ChannelCount,path);

	delete[] data;
	return true;
}


void Texture::TrasitionFormat(bool Write,VkImageLayout OldLayout, VkImageLayout NewLayout,VkCommandBuffer CommandBuffer)
{
	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.oldLayout = OldLayout;
	barrier.newLayout = NewLayout;
	barrier.image = m_Image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = m_AspectMask;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;

	VkPipelineStageFlags SourceStage{};
	VkPipelineStageFlagBits DstStage{};

	if (Write) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		DstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (!Write) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	}


	vkCmdPipelineBarrier(CommandBuffer, SourceStage, DstStage, 0, 0, 0, 0, 0, 1, &barrier);
	m_Layout = NewLayout;	

}

void Texture::CopyFromBuffer(VkDevice device, Buffer* srcbuffer, VkCommandBuffer commandbuffer)
{
	VkBufferImageCopy region{  };
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;

	region.imageExtent.width = m_Width;
	region.imageExtent.height =m_Height;
	region.imageExtent.depth = 1;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.baseArrayLayer = 0;



	vkCmdCopyBufferToImage(commandbuffer, *srcbuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);


}


Texture::~Texture()
{	

	if(m_TextureAtlasData){
		if(m_TextureAtlasData->Data)
			delete[] m_TextureAtlasData->Data;
		delete m_TextureAtlasData;
	}
	#ifdef DEBUG
		if(!m_Image) Core::Log(ErrorType::Warning,"Image was nullptr{~Texture()}");
		if(!m_DeviceMemory) Core::Log(ErrorType::Warning,"DeviceMemory was nullptr{~Texture()}");
		if(!m_Sampler) Core::Log(ErrorType::Warning,"Sampler was nullptr{~Texture()}");
		if(!m_ImageView) Core::Log(ErrorType::Warning,"ImageView was nullptr{~Texture()}");
	#endif

	vkDestroyImage(m_Context->Device,m_Image,nullptr);
	vkFreeMemory(m_Context->Device, m_DeviceMemory, nullptr);
	vkDestroyImageView(m_Context->Device,m_ImageView,nullptr);
	vkDestroySampler(m_Context->Device,m_Sampler,nullptr);
}

void Texture::CreateImageAndView(VkFormat Format,VkSharingMode ShareMode,VkImageTiling ImageTilling,VkImageUsageFlags UsageFlags,VkMemoryPropertyFlags MemoryPropertyFlags,VkImageLayout InitialImageLayout,VkImageAspectFlagBits aspectFlagBits)
{	
	m_ImageFormat = Format;
	m_AspectMask = aspectFlagBits;

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
	imagecreateinfo.extent.height =m_Height;
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
	m_ImageView = CreateView(Format, aspectFlagBits, m_Context->Device);
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

unsigned char* Texture::LoadTextureDataFromFile(std::string Path,uint64_t* Width,uint64_t* Height)
{
	std::string RealPath = Path;
	stbi_uc* image = stbi_load(RealPath.c_str(), (int*)Width, (int*)Height, (int*)&m_ChannelCount, STBI_rgb_alpha);
	if (!image)
	{
		Core::Log(ErrorType::Error, "Failed to load texture file ", RealPath);
		return nullptr;
	}
	return (unsigned char*)image;
}

void Texture::CreateTexture(VkFormat format,VkSharingMode shareMode,VkImageTiling imageTilling,VkImageUsageFlags usageFlags,VkMemoryPropertyFlags memoryPropertyFlags,VkImageLayout initLayout, VkImageLayout finalLayout,void* initData,VkImageAspectFlagBits aspectFlagBits)
{
	static float Anisotropy{ 0 };
	
	VkDeviceSize texturesize = m_Width * m_Height * CalculateBytesPerPixel(format);
	//create stagging buffer
	BufferDesc bufferdesc{};
	bufferdesc.Device = m_Context->Device;
	bufferdesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	bufferdesc.Physdevice = m_Context->PDevice;
	bufferdesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
	bufferdesc.SizeBytes = texturesize;
	bufferdesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	m_Layout = finalLayout;


	if(initData||m_TextureType == TextureType::Texture){
		Buffer stagging(bufferdesc);
	CreateImageAndView(format,shareMode,imageTilling,VK_IMAGE_USAGE_TRANSFER_DST_BIT|usageFlags, memoryPropertyFlags,initLayout,aspectFlagBits);

	stagging.UploadToBuffer(m_Context->Device, initData, texturesize);

	//Create texture and view


	VkCommandBuffer TempCommandBuffer = CommandBuffer::StartSingleUseCommandBuffer(m_Context, m_Context->CommandPool);

	TrasitionFormat(true,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TempCommandBuffer);


	CopyDataFromBuffer(TempCommandBuffer, *stagging.GetBuffer(), m_Image);

	TrasitionFormat(false,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, finalLayout, TempCommandBuffer);

	CommandBuffer::EndSingleUseCommandBuffer(m_Context, m_Context->CommandPool, TempCommandBuffer);
	}
	else{
		CreateImageAndView(format,shareMode,imageTilling,usageFlags, memoryPropertyFlags,initLayout,aspectFlagBits);

	}
	//Maybe something with the spacing or placing of the quad that houses the texture.
	//Create Sampler //TEMP
	VkSamplerCreateInfo samplercreateinfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplercreateinfo.minFilter = VK_FILTER_NEAREST;
	samplercreateinfo.magFilter = VK_FILTER_NEAREST;
	samplercreateinfo.anisotropyEnable = false;
	samplercreateinfo.maxAnisotropy = Anisotropy;
	samplercreateinfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplercreateinfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplercreateinfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplercreateinfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplercreateinfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
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

void Texture::CreateTextureAtlasAndParent(const std::string& MetaData,uint64_t Width,uint64_t Height)
{

	
}


	void Texture::DestroyView(){
		vkDestroyImageView(m_Context->Device,m_ImageView,nullptr);
	}

void Texture::CreateTextureAtlas(uint32_t WidthOfOneTexture, uint32_t HeightOfOneTexture, uint32_t NumOfTexture)
{
	/*
	Float2 OneTextureSizeNormalized{ WidthOfOneTexture / m_Width,HeightOfOneTexture / m_Height };
	uint32_t NumOfTexturesX{std::min(m_Width/ WidthOfOneTexture,NumOfTexture )};
	uint32_t NumOfTexturesY{ NumOfTexture / NumOfTexturesX };

	m_TextureAtlasData = new TextureAtlasCoords[NumOfTexture];
	m_TextureCount = NumOfTexture;

	for (uint32_t y = 0; y < NumOfTexturesY;y++) {
		for (uint32_t x = 0; x < NumOfTexturesX; x++) {
			m_TextureAtlasData[x+(y* NumOfTexturesX)].SizeX = WidthOfOneTexture;
			m_TextureAtlasData[x + (y * NumOfTexturesX)].SizeY = HeightOfOneTexture;

			m_TextureAtlasData[x + (y * NumOfTexturesX)].Points[0] = { x * OneTextureSizeNormalized.x,y * OneTextureSizeNormalized.y };
			Core::Log(ErrorType::Error, "Not finished");

		}
	}
	*/
	//memcpy(m_TextureAtlasData, AtlasCoords.data(), sizeof(TextureAtlasCoords) * NumOfTexture);
	Core::Log(ErrorType::Error, "Replace function");


}



void Texture::CopyDataFromBuffer(VkCommandBuffer CommandBuffer,VkBuffer BufferSrc,VkImage ImageDst)
{
	VkBufferImageCopy region{  };
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight =0;

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

 TextureAtlasData* Texture::CreateTextureAtlasData(const std::string& MetaDataPath) {
	uint64_t Offset{};
	TextureAtlasData* DataRet{};
	std::vector<TextureCoords> AtlasCoords{};
	//Parent texture width and height


	uint64_t Width{}, Height{};

	uint64_t TileLocX{};
	uint64_t TileLocY{};

	uint32_t TileSizeX{};
	uint32_t TileSizeY{};


	std::string Data{};
	std::string Member{};
	uint64_t DataSize{};
	std::ifstream input(MetaDataPath);
	if (!input.is_open()) {
		Core::Log(ErrorType::Error, "Failed to create texture atlas invalid PathToMetaData.");
		return nullptr;
	}
	input.seekg(0, input.end);
	DataSize = input.tellg();
	Data.resize(DataSize);
	input.seekg(0, input.beg);
	input.read(Data.data(), DataSize);


	Offset=Data.find("\"size\"",Offset);
	Offset += ARRAYSIZE("\"size\"") + 8;
	Width = std::stoi(Data.substr(Offset, Data.find(",",Offset) - Offset));
	Offset += Data.find(':', Offset)- Offset+1;

	Height = std::stoi(Data.substr(Offset, Data.find("}", Offset) - Offset));

	Offset = 0;

		Offset += Data.find("\"frame\"", Offset);



	while (Offset != (uint64_t)-1) {
		TextureCoords coords{};
		uint64_t tempOffset{};


		Offset = Data.find("x", Offset) + 4;
		if(Offset == std::string::npos){
			Core::Log(ErrorType::Error,"Couldn't find string inside another string{",MetaDataPath,"}");
			return nullptr;
		}
		TileLocX = std::stoi(Data.substr(Offset, Data.find(",",Offset) - Offset));
		Offset = Data.find("y", Offset) + 4;
		if(Offset == std::string::npos){
			Core::Log(ErrorType::Error,"Couldn't find string inside another string{",MetaDataPath,"}");
			return nullptr;
		}
		TileLocY = std::stoi(Data.substr(Offset, Data.find(",",Offset) - Offset));
		Offset = Data.find("w", Offset) + 4;
		if(Offset == std::string::npos){
			Core::Log(ErrorType::Error,"Couldn't find string inside another string{",MetaDataPath,"}");
			return nullptr;
		}
		TileSizeX = std::stoi(Data.substr(Offset, Data.find(",",Offset) - Offset));
		Offset = Data.find("h", Offset) + 4;
		if(Offset == std::string::npos){
			Core::Log(ErrorType::Error,"Couldn't find string inside another string{",MetaDataPath,"}");
			return nullptr;
		}
		tempOffset= Data.find('}',Offset);
		if(tempOffset == std::string::npos){
			Core::Log(ErrorType::Error,"Couldn't find string inside another string{",MetaDataPath,"} index ",Offset ,(char)Data[Offset]);
			return nullptr;
		}
		TileSizeY = std::stoi(Data.substr(Offset, tempOffset- Offset));
	
		coords.Coords[0] = { (float)TileLocX / (float)Width,1.0f - (float)TileLocY / (float)Height };
		coords.Coords[1] = { (float)TileLocX / (float)Width,1.0f - ((float)TileLocY + TileSizeY) / (float)Height };
		coords.Coords[2] = { (float)(TileLocX + TileSizeX) / (float)Width,1.0f - (float)(TileLocY + TileSizeY) / (float)Height };
		coords.Coords[3] = { (float)(TileLocX + TileSizeX) / (float)Width,1.0f - (float)TileLocY / (float)Height };

		coords.Width = TileSizeX;
		coords.Height = TileSizeY;

		AtlasCoords.push_back(coords);
		Offset = Data.find("\"frame\"", Offset);

	}
	//

	///here
	m_TextureAtlasData = new TextureAtlasData();

	m_TextureAtlasData->Data = new TextureCoords[AtlasCoords.size()];
	m_TextureCount = (uint32_t)AtlasCoords.size();
	memcpy(m_TextureAtlasData->Data, AtlasCoords.data(), sizeof(TextureCoords) * AtlasCoords.size());
	return DataRet;
}
