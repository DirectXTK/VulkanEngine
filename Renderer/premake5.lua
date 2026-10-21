project "Renderer"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

    RendererLib="../bin".. outputdir.."/%{prj.name}"

    files{
        "Source/**.cpp",
        "Source/**.h"
    }

    defines{
    }

    includedirs{
      "%{prj.location}/../Application/Source/",
      "%{prj.location}/Source/",
      vulkanSDK.."/Include/",
      "%{prj.location}/../Include",
      GlobalIncludes,
    }

     libdirs{
        "%{prj.location}/../Libs/",
        vulkanSDK.."/Lib/",
    }

    links{
      "vulkan-1",
      "glfw3",
      "freetype",
      "SPIRV",
    "glslang",
    "MachineIndependent",
    "GenericCodeGen",
    "OSDependent",
    "OGLCompiler",

    "SPIRV-Tools",
    "SPIRV-Tools-opt",
    "SPIRV-Tools-link",
    }
    postbuildcommands {
    "{MKDIR} %[" .. LibLocation .. "]",
    "{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[" .. LibLocation .. "]"
    }


    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"

    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"

    filter{"configurations:Distrib"}
      defines {"DISTRIB"}
      defines { "NDEBUG" }     
      optimize "Full"          
      symbols "Off"             
      runtime "Release"         
      staticruntime "off"      
      linktimeoptimization "On"

