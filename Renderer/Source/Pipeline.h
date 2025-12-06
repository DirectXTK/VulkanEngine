#pragma once
#include "RendCore.h"
#include "Shader.h"
struct VertexStageInputAttrib {
	VkFormat Format{};
	uint32_t Offset{};
	uint32_t Location{};
	uint32_t Binding{};
};
struct PipelineDesc {
	VkRenderPass RenderPass{};
	uint32_t VertexStageInputCount{};
	VertexStageInputAttrib* VertexStageInput{};
	VkViewport Viewport{};
	VkPipelineLayout PipelineLayout{};
	uint32_t VertexInputStride{};
	//fiil wireframe
	VkPolygonMode RenderType{VK_POLYGON_MODE_FILL};
	VkPrimitiveTopology Topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
	bool Blending{true};

	Shader* Shaders{};
	uint32_t ShaderCount{};
};

class Pipeline {
public:
	static VkPipelineLayout CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout* DescriptorSetLayout,uint32_t DescriptorSetCount);
	static VkPipeline CreatePipeline(const PipelineDesc& desc, VkDevice device);
	static VkRenderPass CreateRenderPass(VkDevice device, VkFormat format);
	static void CreateDepthStencil(VkDevice device);
private:

};