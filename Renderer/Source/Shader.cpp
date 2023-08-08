#include "Shader.h"

       Shader::Shader(const char* Path,VkDevice device){
                std::ifstream file(Path,std::ios::binary);
                std::vector<char> ByteCode{};
                uint32_t Size{};
                
                m_Device =device;

                file.seekg(0,file.end);

                Size = file.tellg();
                ByteCode.resize(Size);


                file.seekg(0,file.beg);

                file.read(ByteCode.data(),Size);

                VkShaderModuleCreateInfo info{};
                info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                info.codeSize = Size;
                info.pCode = (uint32_t*)ByteCode.data();


                VkResult result = vkCreateShaderModule(device,&info,nullptr,&m_ShaderModule);
                if(result != VK_SUCCESS)
                      Core::Log(ErrorType::Error,"Failed to create shader module.");

       }


        Shader::~Shader(){

              vkDestroyShaderModule(m_Device,m_ShaderModule,nullptr);
        }
