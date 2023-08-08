workspace "VulkanEngine"
    architecture "x64"

    configurations{
        "Debug",
        "Release"
    }
    
    startproject "Application"
    startproject "Renderer"

outputdir = "%{cfg.build}/%{cfg.system}/%{cfg.architecture}"

include "Application"
include "Renderer"
