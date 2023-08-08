#pragma once
#include "RendCore.h"

class Shader{
    public:
        Shader(const char* Path,VkDevice device);

        VkShaderModule GetShaderModule(){return m_ShaderModule;}
        ~Shader();
    private:
        VkShaderModule m_ShaderModule{};
        VkDevice m_Device{};
};