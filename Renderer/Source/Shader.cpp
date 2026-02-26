#include "Shader.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/Public/ResourceLimits.h"
//#include "glslang/ResourceLimits/ResourceLimits.cpp"
#include "glslang/SPIRV/GlslangToSpv.h"

TBuiltInResource DefaultTBuiltInResource = {
    /* maxLights */ 32,
    /* maxClipPlanes */ 6,
    /* maxTextureUnits */ 32,
    /* maxTextureCoords */ 32,
    /* maxVertexAttribs */ 64,
    /* maxVertexUniformComponents */ 4096,
    /* maxVaryingFloats */ 64,
    /* maxVertexTextureImageUnits */ 32,
    /* maxCombinedTextureImageUnits */ 80,
    /* maxTextureImageUnits */ 32,
    /* maxFragmentUniformComponents */ 4096,
    /* maxDrawBuffers */ 32,
    /* maxVertexUniformVectors */ 128,
    /* maxVaryingVectors */ 8,
    /* maxFragmentUniformVectors */ 16,
    /* maxVertexOutputVectors */ 16,
    /* maxFragmentInputVectors */ 15,
    /* minProgramTexelOffset */ -8,
    /* maxProgramTexelOffset */ 7,
    /* maxClipDistances */ 8,
    /* maxComputeWorkGroupCountX */ 65535,
    /* maxComputeWorkGroupCountY */ 65535,
    /* maxComputeWorkGroupCountZ */ 65535,
    /* maxComputeWorkGroupSizeX */ 1024,
    /* maxComputeWorkGroupSizeY */ 1024,
    /* maxComputeWorkGroupSizeZ */ 64,
    /* maxComputeUniformComponents */ 1024,
    /* maxComputeTextureImageUnits */ 16,
    /* maxComputeImageUniforms */ 8,
    /* maxComputeAtomicCounters */ 8,
    /* maxComputeAtomicCounterBuffers */ 1,
    /* maxVaryingComponents */ 60,
    /* maxVertexOutputComponents */ 64,
    /* maxGeometryInputComponents */ 64,
    /* maxGeometryOutputComponents */ 128,
    /* maxFragmentInputComponents */ 128,
    /* maxImageUnits */ 8,
    /* maxCombinedImageUnitsAndFragmentOutputs */ 8,
    /* maxCombinedShaderOutputResources */ 8,
    /* maxImageSamples */ 0,
    /* maxVertexImageUniforms */ 0,
    /* maxTessControlImageUniforms */ 0,
    /* maxTessEvaluationImageUniforms */ 0,
    /* maxGeometryImageUniforms */ 0,
    /* maxFragmentImageUniforms */ 8,
    /* maxCombinedImageUniforms */ 8,
    /* maxGeometryTextureImageUnits */ 16,
    /* maxGeometryOutputVertices */ 256,
    /* maxGeometryTotalOutputComponents */ 1024,
    /* maxGeometryUniformComponents */ 1024,
    /* maxGeometryVaryingComponents */ 64,
    /* maxTessControlInputComponents */ 128,
    /* maxTessControlOutputComponents */ 128,
    /* maxTessControlTextureImageUnits */ 16,
    /* maxTessControlUniformComponents */ 1024,
    /* maxTessControlTotalOutputComponents */ 4096,
    /* maxTessEvaluationInputComponents */ 128,
    /* maxTessEvaluationOutputComponents */ 128,
    /* maxTessEvaluationTextureImageUnits */ 16,
    /* maxTessEvaluationUniformComponents */ 1024,
    /* maxTessPatchComponents */ 120,
    /* maxPatchVertices */ 32,
    /* maxTessGenLevel */ 64,
    /* maxViewports */ 16,
    /* maxVertexAtomicCounters */ 0,
    /* maxTessControlAtomicCounters */ 0,
    /* maxTessEvaluationAtomicCounters */ 0,
    /* maxGeometryAtomicCounters */ 0,
    /* maxFragmentAtomicCounters */ 8,
    /* maxCombinedAtomicCounters */ 8,
    /* maxAtomicCounterBindings */ 1,
    /* maxVertexAtomicCounterBuffers */ 0,
    /* maxTessControlAtomicCounterBuffers */ 0,
    /* maxTessEvaluationAtomicCounterBuffers */ 0,
    /* maxGeometryAtomicCounterBuffers */ 0,
    /* maxFragmentAtomicCounterBuffers */ 1,
    /* maxCombinedAtomicCounterBuffers */ 1,
    /* maxAtomicCounterBufferSize */ 16384,
    /* maxTransformFeedbackBuffers */ 4,
    /* maxTransformFeedbackInterleavedComponents */ 64,
    /* maxCullDistances */ 8,
    /* maxCombinedClipAndCullDistances */ 8,
    /* maxSamples */ 4,

    /* maxMeshOutputVerticesNV */ 256,
    /* maxMeshOutputPrimitivesNV */ 512,
    /* maxMeshWorkGroupSizeX_NV */ 32,
    /* maxMeshWorkGroupSizeY_NV */ 1,
    /* maxMeshWorkGroupSizeZ_NV */ 1,
    /* maxTaskWorkGroupSizeX_NV */ 32,
    /* maxTaskWorkGroupSizeY_NV */ 1,
    /* maxTaskWorkGroupSizeZ_NV */ 1,
    /* maxMeshViewCountNV */ 4,

    /* maxMeshOutputVerticesEXT */ 256,
    /* maxMeshOutputPrimitivesEXT */ 512,
    /* maxMeshWorkGroupSizeX_EXT */ 32,
    /* maxMeshWorkGroupSizeY_EXT */ 1,
    /* maxMeshWorkGroupSizeZ_EXT */ 1,
    /* maxTaskWorkGroupSizeX_EXT */ 32,
    /* maxTaskWorkGroupSizeY_EXT */ 1,
    /* maxTaskWorkGroupSizeZ_EXT */ 1,
    /* maxMeshViewCountEXT */ 4,

    /* maxDualSourceDrawBuffersEXT */ 1,

    /* limits */
    {
        /* nonInductiveForLoops */ 1,
        /* whileLoops */ 1,
        /* doWhileLoops */ 1,
        /* generalUniformIndexing */ 1,
        /* generalAttributeMatrixVectorIndexing */ 1,
        /* generalVaryingIndexing */ 1,
        /* generalSamplerIndexing */ 1,
        /* generalVariableIndexing */ 1,
        /* generalConstantMatrixVectorIndexing */ 1,
    }
};

    Shader::Shader(const std::string& path,VkDevice device): m_Device(device){
         glslang::InitializeProcess();

        std::string extension = Core::GetFileExtension(path);
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

        std::string outputShaderPath = m_Desc.Path.substr(0,m_Desc.Path.size()-(m_Desc.Path.size()- m_Desc.Path.find('.',0)));
        outputShaderPath+= ".spv";

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
                Core::Log(ErrorType::Error,"Invalid type{GetShaderStage}:",(uint32_t)m_Desc.Type);
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
        std::ifstream input(filePath);
        std::string data{};
        uint64_t size{};
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
        EShMessages messages = (EShMessages)(EShMsgVulkanRules|EShMsgSpvRules|EShMsgDefault);
        
        const char* ptr=  data.c_str(); 
        shader.setStrings(&ptr,1);
        shader.setEnvInput(glslang::EShSourceGlsl, type, glslang::EShClientVulkan, 460);
        shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_3);
        shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);
        if(!shader.parse(&DefaultTBuiltInResource,460,false,messages)){
            Core::Log(ErrorType::Error,"Shader parsing failed:\n",shader.getInfoLog(),shader.getInfoDebugLog(),m_Desc.Path);
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
        //Core::Log("Size",spirv.size(),outputPath);
        
        std::ofstream output(outputPath,std::ios::binary);
        output.write((char*)spirv.data(),spirv.size()*4);

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

