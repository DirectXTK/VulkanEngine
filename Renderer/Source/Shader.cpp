#include "Shader.h"

     

        VkShaderModule ShaderDesc::CreateShader(const ShaderDesc& desc,VkDevice device)
        {
            std::ifstream file(desc.Path, std::ios::binary);
            std::vector<char> ByteCode{};
            VkShaderModule ShaderModule{};
            uint64_t Size{};


            file.seekg(0, file.end);

            Size = file.tellg();
            ByteCode.resize(Size);


            file.seekg(0, file.beg);

            file.read(ByteCode.data(), Size);

            VkShaderModuleCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.codeSize = Size;
            info.pCode = (uint32_t*)ByteCode.data();


            VkResult result = vkCreateShaderModule(device, &info, nullptr, &ShaderModule);
            if (result != VK_SUCCESS)
                Core::Log(ErrorType::Error, "Failed to create shader module.");
            return ShaderModule;

        }
