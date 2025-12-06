#include "Shader.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/Public/ResourceLimits.h"
TBuiltInResource DefaultTBuiltInResource = {};

void FillDefaultResources()
{
    DefaultTBuiltInResource = {};

    // The only values glslang *actually needs* for Vulkan:
    DefaultTBuiltInResource.maxDrawBuffers = 32;
    DefaultTBuiltInResource.maxTextureUnits = 32;
    DefaultTBuiltInResource.maxTextureCoords = 32;

    // Set sane defaults for limits
    DefaultTBuiltInResource.limits.nonInductiveForLoops = true;
    DefaultTBuiltInResource.limits.whileLoops = true;
    DefaultTBuiltInResource.limits.doWhileLoops = true;
    DefaultTBuiltInResource.limits.generalUniformIndexing = true;
    DefaultTBuiltInResource.limits.generalAttributeMatrixVectorIndexing = true;
    DefaultTBuiltInResource.limits.generalVaryingIndexing = true;
    DefaultTBuiltInResource.limits.generalSamplerIndexing = true;
    DefaultTBuiltInResource.limits.generalVariableIndexing = true;
    DefaultTBuiltInResource.limits.generalConstantMatrixVectorIndexing = true;
}

    Shader::Shader(const std::string& path,VkDevice device): m_Device(device){
        std::string extension = Core::GetFileExtension(path);
             FillDefaultResources();
        glslang::InitializeProcess();
        m_Desc.Path = path;
        if(extension == "spv"){
            m_Desc.binary = true;
            CreateShader();
            return;
        }


        if(extension == "fragS"){
            m_Desc.Type = ShaderType::Pixel;
        }
       else  if(extension == "vertS"){
            m_Desc.Type = ShaderType::Vertex;
        }
       else  if(extension == "geomS"){
            m_Desc.Type = ShaderType::Geometry;
        }
       else  if(extension == "taskS"){
            m_Desc.Type = ShaderType::Task;
        }
       else  if(extension == "meshS"){
            m_Desc.Type = ShaderType::Mesh;

        }
        else if(extension == "computeS"){
            m_Desc.Type = ShaderType::Compute;

        }else{

            Core::Log(ErrorType::Error,"Invalid type{Shader::Shader}:",extension);
        }

        std::string outputShaderPath = m_Desc.Path.substr(0,m_Desc.Path.size()- m_Desc.Path.find('.',0)+1);
        outputShaderPath+= "spv";
       if(CompileShader(m_Desc.Path,outputShaderPath)){
           m_Desc.Path = outputShaderPath;
           CreateShader();

       }
    
    }
    Shader::Shader(char* data,uint64_t sizeInBytes,const ShaderType& shaderType,VkDevice device): m_Device(device){

    }
    VkShaderStageFlagBits Shader::GetShaderStage(){
        switch(m_Desc.Type){
            case ShaderType::Vertex:{
                return VK_SHADER_STAGE_VERTEX_BIT;
                break;
            }
             case ShaderType::Pixel:{
                return VK_SHADER_STAGE_FRAGMENT_BIT;
                break;
            }
             case ShaderType::Geometry:{
                return VK_SHADER_STAGE_GEOMETRY_BIT;
                break;
            }
             case ShaderType::Compute:{
                return VK_SHADER_STAGE_COMPUTE_BIT;
                break;
            }
             case ShaderType::Task:{
                return VK_SHADER_STAGE_TASK_BIT_EXT;
                break;
            }
             case ShaderType::Mesh:{
                return VK_SHADER_STAGE_MESH_BIT_EXT;
                break;
            }
            default :{
                Core::Log(ErrorType::Error,"Invalid type{GetShaderState}:",(uint32_t)m_Desc.Type);
                break;
            }
        }
        return VK_SHADER_STAGE_ALL;
    }
    bool Shader::CompileShader(const std::string& filePath,const std::string& outputPath){
   
        EShLanguage type{EShLangVertex};
        switch(m_Desc.Type){
            case ShaderType::Vertex:{
                type = EShLangVertex;
                break;
            }
             case ShaderType::Pixel:{
                type = EShLangFragment;
                break;
            }
             case ShaderType::Geometry:{
                type = EShLangGeometry;
                break;
            }
             case ShaderType::Compute:{
                type = EShLangCompute;
                break;
            }
             case ShaderType::Mesh:{
                type = EShLangMesh;
                break;
            }
             case ShaderType::Task:{
                type = EShLangTask;
                break;
            }
            default :{
                Core::Log("Invalid type{Shader::CompileShader}");
                break;
            }
        }
        glslang::TShader shader(type);
        std::ifstream input(filePath,std::ios::binary);
        std::string data{};
        uint64_t size{};
        Core::Log(filePath," ",(uint32_t)type);
        if(!input.is_open())
        {
            Core::Log(ErrorType::Error,"Failed to compile shader{Shader::CompileShader}: ",filePath);
            return false;
        }
        input.seekg(0,input.end);
        size = input.tellg();
        data.resize(size);
        input.seekg(0,input.beg);
        input.read(data.data(),size);
        //Compile together
        EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);
        
        const char* ptr=  data.c_str();
        shader.setStrings(&ptr,1);
        shader.setEnvInput(glslang::EShSourceGlsl, type, glslang::EShClientVulkan, 460);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
        shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
        if(!shader.parse(&DefaultTBuiltInResource,460,false,messages)){
            Core::Log(ErrorType::Error,"Shader parsing failed:\n",shader.getInfoLog());
            return false;
        }
        glslang::TProgram program;
        program.addShader(&shader);

        if(!program.link(EShMsgDefault)){
            Core::Log(ErrorType::Error,"Shader linking failed:\n",program.getInfoLog());
            return false;
        }

        std::vector<uint32_t> spirv{};
        glslang::GlslangToSpv(*program.getIntermediate(type),spirv);
        
        //std::ofstream output(outputPath);
       // output<< spirv.data();

        return true;
    }
     

    bool Shader::CreateShader()
    {
            std::ifstream file(m_Desc.Path, std::ios::binary);
            std::vector<char> ByteCode{};
            uint64_t Size{};

            if(!file.is_open())
            {
                    Core::Log(ErrorType::Error,"Can't open file ",m_Desc.Path);
                    return false; 
            }
            file.seekg(0, file.end);

            Size = file.tellg();
            ByteCode.resize(Size);


            file.seekg(0, file.beg);

            file.read(ByteCode.data(), Size);

            VkShaderModuleCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            info.codeSize = Size;
            info.pCode = (uint32_t*)ByteCode.data();

            VkResult result = vkCreateShaderModule(m_Device, &info, nullptr, &m_Module);
            if (result != VK_SUCCESS){
                Core::Log(ErrorType::Error, "Failed to create shader module.");
                return false;
            }
            return true;

    }
    Shader::~Shader(){
        vkDestroyShaderModule(m_Device,m_Module,nullptr);
        glslang::FinalizeProcess();

    }

