workspace "VulkanEngine"
    architecture "x64"
    cppdialect "C++17"
    characterset ("Unicode")
    toolset "clang"
    buildoptions("-MP")
    configurations{
        "Debug",
        "Release",
        "Distrib"
    }
    linkoptions {
    "-Wl,--start-group",
    "-lApplication",
    "-lRenderer",
    "-Wl,--end-group",
    }
    filter{"configurations:Release"}
      defines {"RELEASE"}
    filter{"configurations:Debug"}
      defines {"DEBUG"}
    filter{"configurations:Distrib"}
      defines {"DISTRIB"}

    
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

