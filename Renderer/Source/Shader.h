#pragma once
#include "RendCore.h"
enum class ShaderType{PixelShader,VertexShader,GeometryShader};
struct ShaderDesc {
    ShaderType Type{};
    const char* Path{};

    static VkShaderModule CreateShader(const ShaderDesc& desc,VkDevice device);
};
