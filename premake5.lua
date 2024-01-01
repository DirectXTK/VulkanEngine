workspace "VulkanEngine"
    architecture "x64"

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

