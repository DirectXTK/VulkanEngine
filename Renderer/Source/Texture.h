#pragma once
#include "RendCore.h"
#include "Buffer.h"
#include "Context.h"
//rework texture class for depth stencil and color attachment !!
struct TextureCreateInfo {
	VkFormat Format{ VK_FORMAT_R8G8B8A8_UNORM };
	VkImageLayout ImageLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
	VkSharingMode SharingMode{ VK_SHARING_MODE_EXCLUSIVE };
	VkImageTiling ImageTilling{ VK_IMAGE_TILING_OPTIMAL };
	VkImageUsageFlagBits ImageUsageFlags{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
	VkMemoryPropertyFlagBits MemoryPropertyFlags{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
	uint32_t Width{};
	uint32_t Height{};
	uint32_t ChannelCount{};
	void* Pixels{};
};
enum class TextureType{UNDIFINED,Texture,TextureAtlas,ColorAttachment,DepthStencilAttachment,SwapChainImage};
enum class TextureCreateFlagBits {
	DEFAULT=0,CREATEATLAS=2,
};
struct TextureCoords {
	Float2 Coords[4];
	uint64_t Width, Height{};

};
struct TextureAtlasData {
	TextureCoords* Data{};
	uint32_t TextureCount{};
	GUUID TextureID{};
};
class Texture
{
public:
	Texture(Context context,const TextureCreateInfo& createInfo,std::string Path,TextureType type=TextureType::Texture);
	

	//predifined constructors for easier creation of simple texture types

	Texture(Context context, const TextureCreateInfo& createInfo,const TextureType& type);


	//Used for texture atlases.
	Texture(Texture* texture, uint32_t TextureIndex);
	//for swapChain
	Texture(Context context,VkFormat format,VkImage image);


	void TrasitionFormat(bool Write,VkImageLayout OldLayout, VkImageLayout NewLayout, VkCommandBuffer CommandBuffer);
	void CopyFromBuffer(VkDevice device, Buffer* srcbuffer, VkCommandBuffer commandbuffer);

	VkImageView GetImageView() { return m_ImageView; }
	VkImage GetImage() { return m_Image; }
	VkSampler GetSampler() { return m_Sampler; }

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }
	//texture atlas
	//returns 4 points

	//TextureAtlasCoords* GetSubTextureData(uint32_t TextureIndex) { return &m_TextureData->m_TextureAtlasData[TextureIndex]; }
	uint64_t GetByteSize(){return (uint64_t)m_DeviceSize;}

	Texture** GetTextureAtlas();
	//Small texture inside this bigger texture.
	uint32_t GetTextureAtlasSize();
	void CreateTextureAtlas(uint32_t WidthOfOneTexture, uint32_t HeightOfOneTexture, uint32_t NumOfTexture);
	//used for differentSizedTextures
	//void CreateTextureAtlas(TextureAtlasCoords* coords, uint32_t NumOfTexture,Float2* SubTextureSizes);

	static TextureAtlasData* CreateTextureAtlasData(const std::string& MetaDataPath);

	~Texture();
private:

	VkImageView CreateView(VkFormat Format,VkImageAspectFlagBits AspectFlags,VkDevice Device);
	void CreateImageAndView(VkFormat Format, VkSharingMode ShareMode, VkImageTiling ImageTilling, VkImageUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags, VkImageLayout InitialImageLayout,VkImageAspectFlagBits aspectFlagBits);
	unsigned char* LoadTextureDataFromFile(std::string Path,uint64_t* Width,uint64_t* Height);
	void CreateTexture(VkFormat format,VkSharingMode shareMode,VkImageTiling imageTilling,VkImageUsageFlags usageFlags,VkMemoryPropertyFlags memoryPropertyFlags,VkImageLayout initialImageLayout,VkImageLayout finalLayout,void* initData=nullptr,VkImageAspectFlagBits aspectFlagBits= VK_IMAGE_ASPECT_COLOR_BIT );

	void CreateTextureAtlasAndParent(const std::string& MetaData,uint64_t Width,uint64_t Height);


	void CopyDataFromBuffer(VkCommandBuffer CommandBuffer, VkBuffer BufferSrc, VkImage ImageDst);

	
	VkImage m_Image{};
	VkImageView m_ImageView{};
	uint32_t m_ChannelCount{};
	Context m_Context{};
	VkSampler m_Sampler{  };

	VkDeviceMemory m_DeviceMemory{};
	VkDeviceSize m_DeviceSize{};

	uint64_t m_Width{}, m_Height{};

	VkImageAspectFlagBits m_AspectMask{};
	uint32_t m_TextureIndex{};
	TextureType m_TextureType{};


};
class TextureAtlas {
public:
	//use this if size of textures are the same and they are packed and the size is known
	TextureAtlas(Context context,Float2 OneTextureSize, Texture* texture);
	TextureAtlas(Context context, std::string PathToMetaData,Texture* texture);


	Float2* GetTexCoords(uint32_t Index);
	uint32_t GetOneTextureWidth(uint32_t Index) { return m_TextureAtlasData[Index].SizeX; }
	uint32_t GetOneTextureHeight(uint32_t Index) { return m_TextureAtlasData[Index].SizeY; }

	Texture* GetTexture() { return m_Texture; }

	~TextureAtlas();
private:
	struct TextureAtlasCoords {
		Float2 Points[4];
		uint32_t SizeX, SizeY{};
	};
	uint32_t m_TextureCount{};
	TextureAtlasCoords* m_TextureAtlasData{};
	Texture* m_Texture{};
};

