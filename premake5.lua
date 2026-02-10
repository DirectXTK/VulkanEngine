workspace "VulkanEngine"
    architecture "x64"
    cppdialect "C++17"
    characterset ("Unicode")
    toolset "clang"
    buildoptions { "-MP"}
    configurations{
        "Debug",
        "Release",
        "Distrib",
        "Verbose"
    }
    linkoptions {
    "-Wl,--start-group",
    "-lApplication",
    "-lRenderer",
    "-Wl,--end-group",
    }
    filter { "system:linux" }
      defines{"LINUX"}
    filter { "system:windows"}
      defines{"WINDOWS"}
    filter{"configurations:Release"}
      defines {"RELEASE"}
      runtime "Release"
      optimize "on"
    filter{"configurations:Debug"}
      defines {"DEBUG"}
    filter{"configurations:Verbose"}
      buildoptions {
      "-fsanitize=address",
      "-fsanitize=undefined",
      "-fno-omit-frame-pointer"
      }
      linkoptions {
      "-fsanitize=address",
      "-fsanitize=undefined"
      }
      defines {"DEBUG"}
    filter{"configurations:Distrib"}
      defines {"DISTRIB"}
      defines { "NDEBUG" }     
      optimize "Full"          
      symbols "Off"             
      runtime "Release"         
      staticruntime "On"      
      linktimeoptimization "On"

    
    startproject "Application"
    startproject "Renderer"
    startproject "Sandbox"

LibLocation="%{wks.location}/Libs/"
outputdir = "/%{cfg.buildcfg}/%{cfg.system}/%{cfg.architecture}"
vulkanSDK = os.getenv("VULKAN_SDK")
GlobalIncludes = {
    "%{prj.location}/../Include/FreeType",
}

include "Application"
include "Renderer"
include "Sandbox"

