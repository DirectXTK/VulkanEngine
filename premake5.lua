workspace "VulkanEngine"
    architecture "x64"
    cppdialect "C++17"
    characterset ("Unicode")
    toolset "clang"
    buildoptions("-MP")
    configurations{
        "Debug",
        "Release"
    }
    linkoptions {
    "-Wl,--start-group",
    "-lApplication",
    "-lRenderer",
    "-Wl,--end-group",
    }   
    
    startproject "Application"
    startproject "Renderer"
    startproject "Sandbox"

LibLocation="%{wks.location}/Libs/"
outputdir = "%{cfg.build}/%{cfg.system}/%{cfg.architecture}"
vulkanSDK = os.getenv("VULKAN_SDK")
GlobalIncludes = {
    "%{prj.location}/../Include/FreeType",
}

include "Application"
include "Renderer"
include "Sandbox"

