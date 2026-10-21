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
      "%{prj.location}/../vendor/glfw/include",
      GlobalIncludes,
    }

     libdirs{
        "%{prj.location}/../Libs/",
        vulkanSDK.."/Lib/",
    }

    filter{"configurations:Debug"}
           links{
      "vulkan-1",
      "glfw3",
      "freetyped",
      "SPIRVd",
    "glslangd",
    "MachineIndependentd",
    "GenericCodeGend",
    "OSDependentd",
    "OGLCompilerd",
    "SPIRV-Toolsd",
    "SPIRV-Tools-optd",
    "SPIRV-Tools-linkd",
    }
    filter{"configurations:Release"}
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
    filter{"system:windows"}
     prebuildcommands {
      "cd ../vendor/freetype",
      "{MKDIR} build/%[%{!cfg.buildcfg}]",
      'cmake -S . -B build/%[%{!cfg.buildcfg}] -G "Unix Makefiles" -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL',
      'cmake --build build/%[%{!cfg.buildcfg}]',
      '{COPYFILE} build/%[%{!cfg.buildcfg}]/freetype*.lib %[' .. LibLocation ..']',
      "cd ../glfw",
      "{MKDIR} build/%[%{!cfg.buildcfg}]",
      'cmake -S . -B build/%[%{!cfg.buildcfg}] -G "Unix Makefiles" -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL',
      'cmake --build build/%[%{!cfg.buildcfg}]',
      '{COPYFILE} build/%[%{!cfg.buildcfg}]/src/glfw3*.lib %[' .. LibLocation ..']',
     }

    filter{"system:linux"}
     prebuildcommands {
      "cd ../vendor/freetype",
      "{MKDIR} build/%[%{!cfg.buildcfg}]",
      'cmake -S . -B build/%[%{!cfg.buildcfg}] -G "Unix Makefiles"',
      'cmake --build build/%[%{!cfg.buildcfg}]',
      '{COPYFILE} build/%[%{!cfg.buildcfg}]/freetype**.so %[' .. LibLocation ..']'
     }
  
  
   
    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"


    filter{"configurations:Release"}
      runtime "Release"
      optimize "off"

    filter{"configurations:Distrib"}
      defines {"DISTRIB"}
      defines { "NDEBUG" }     
      optimize "Full"          
      symbols "Off"             
      runtime "Release"         
      staticruntime "off"      
      linktimeoptimization "On"

