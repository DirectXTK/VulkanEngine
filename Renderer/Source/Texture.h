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
enum class TextureCreateFlagBits {
	DEFAULT=0,CREATEATLAS=2,
};
struct TextureAtlasCoords {
	Float2 Points[4];
	uint64_t SizeX, SizeY{};
};
class Texture
{
public:
	Texture(Context context,uint32_t Width, uint32_t Height,uint32_t ChannelCount,void* Pixels );
	Texture(Context context,std::string Path);

	//Used for texture atlases.
	Texture(Texture* texture, uint32_t TextureIndex);


	void TrasitionFormat(VkImageLayout OldLayout, VkImageLayout NewLayout, VkCommandBuffer CommandBuffer);
	void CopyFromBuffer(VkDevice device, Buffer* srcbuffer, VkCommandBuffer commandbuffer);

	VkImageView GetImageView() { return m_TextureData->m_ImageView; }
	VkImage GetImage() { return m_TextureData->m_Image; }
	VkSampler GetSampler() { return m_TextureData->m_Sampler; }

	uint32_t GetWidth() { return m_Width; }
	uint32_t GetHeight() { return m_Height; }
	//texture atlas
	//returns 4 points
	Float2* GetTextureCoords() { return m_TextureData->m_TextureAtlasData[m_TextureIndex].Points; }
	Float2* GetSubTextureCoords(uint32_t TextureIndex) { return m_TextureData->m_TextureAtlasData[TextureIndex].Points; }
	Float2 GetSubTextureSize(uint32_t TextureIndex) { return { (float)m_TextureData->m_TextureAtlasData[TextureIndex].SizeX, (float)m_TextureData->m_TextureAtlasData[TextureIndex].SizeY };}

	Texture** GetTextureAtlas();
	//Small texture inside this bigger texture.
	uint32_t GetTextureAtlasSize();
	void CreateTextureAtlas(uint32_t WidthOfOneTexture, uint32_t HeightOfOneTexture, uint32_t NumOfTexture);
	//used for differentSizedTextures
	void CreateTextureAtlas(TextureAtlasCoords* coords, uint32_t NumOfTexture,Float2* SubTextureSizes);


	~Texture();
private:

	VkImageView CreateView(VkFormat Format,VkImageAspectFlagBits AspectFlags,VkDevice Device);
	void CreateImageAndView(VkFormat Format, VkSharingMode ShareMode, VkImageTiling ImageTilling, VkImageUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags, VkImageLayout InitialImageLayout);
	unsigned char* LoadTextureDataFromFile(std::string Path);
	void CreateTexture(void* initData);
	void CreateTextureAtlas(const std::string& MetaData);


	void CopyDataFromBuffer(VkCommandBuffer CommandBuffer, VkBuffer BufferSrc, VkImage ImageDst);

	
	struct TextureData {
		VkImage m_Image{};
		VkImageView m_ImageView{};
		uint32_t m_Width{};
		uint32_t m_Height{};
		uint32_t m_ChannelCount{};
		Context m_Context{};
		VkSampler m_Sampler{  };

		VkDeviceMemory m_DeviceMemory{};
		VkDeviceSize m_DeviceSize{};


		uint32_t m_TextureCount{};
		TextureAtlasCoords* m_TextureAtlasData{};
	};

	uint32_t m_Width{};
	uint32_t m_Height{};
	uint32_t m_TextureIndex{};
	//Texture atlas
	TextureData* m_TextureData{};
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

