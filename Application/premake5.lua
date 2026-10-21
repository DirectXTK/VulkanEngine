project "Application"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

  
    files{
        "Source/**.cpp",
        "Source/**.h"
    }
    


    includedirs{
      "%{prj.location}/../Include",

      "%{prj.location}/../Include/vulkan/",
      "%{prj.location}/../Renderer/Source/",
      "%{prj.location}/Source/",

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
      "Renderer", "glslang","SPIRV-Tools-opt",
        "SPIRV-Tools",
        "SPIRV-Tools-link",
    }

    defines{
    }


    postbuildcommands {
    '{MKDIR} "' .. LibLocation .. '"',
    '{COPYFILE} "%{cfg.buildtarget.abspath}" "' .. LibLocation .. '"'
    }


    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"


    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"
      staticruntime "off"
    filter{"configurations:Distrib"}
      defines {"DISTRIB"}
      defines { "NDEBUG" }     
      optimize "Full"          
      symbols "Off"             
      runtime "Release"         
      staticruntime "off"      
      linktimeoptimization "On"

