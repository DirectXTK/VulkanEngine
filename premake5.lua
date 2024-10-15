workspace "VulkanEngine"
    architecture "x64"
    cppdialect "C++17"
    characterset ("Unicode")
    buildoptions("-MP")
    configurations{
        "Debug",
        "Release"
    }
    
    startproject "Application"
    startproject "Renderer"
    startproject "Sandbox"

outputdir = "%{cfg.build}/%{cfg.system}/%{cfg.architecture}"

include "Application"
include "Renderer"
include "Sandbox"

