#pragma once
#include "RendCore.h"
enum class ShaderType{Pixel,Vertex,Geometry,Task,Compute,Mesh};
struct ShaderDesc {
    ShaderType Type{};

    //if binary = true when its compiled if false it compiles it.
    bool binary{false};
    std::string Path{};

};
class Shader{
    public:
    Shader(const std::string& path,VkDevice device);
    Shader(char* data,uint64_t sizeInBytes,const ShaderType& shaderType,VkDevice device);

    VkShaderModule GetShaderModule(){return m_Module;}
    ShaderType GetType(){return m_Desc.Type;}
    VkShaderStageFlagBits GetShaderStage();

    ~Shader();
    private:
    bool CompileShader(const std::string& filePath,const std::string& outputPath);
    bool CreateShader();
    
    VkShaderModule m_Module{};
    VkDevice m_Device{};
    ShaderDesc m_Desc{};


};
