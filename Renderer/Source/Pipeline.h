#pragma once
#include "RendCore.h"
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
};

class Pipeline {
public:
	static VkPipelineLayout CreatePipelineLayout(VkDevice device, VkDescriptorSetLayout* DescriptorSetLayout);
	static VkPipeline CreatePipeline(PipelineDesc& desc, VkDevice device);
	static VkRenderPass CreateRenderPass(VkDevice device, VkFormat format);

private:

};