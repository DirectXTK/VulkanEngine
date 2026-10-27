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
    "kernel32", "user32", "gdi32", "shell32", "advapi32", "ole32",
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
    local function ThirdPartyBuild(cfg, runtime)
    local ft, glfw = "../vendor/freetype", "../vendor/glfw"
    local common = ' -G "Visual Studio 17 2022" -A x64' ..
                   ' -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. runtime
    -- freetype appends "d" to the lib name in Debug
    local ftLib = (cfg == "Debug") and "freetyped.lib" or "freetype.lib"

    return {
        'cmake -E make_directory "' .. LibLocation .. '"',

        'cmake -S ' .. ft .. ' -B ' .. ft .. '/build/' .. cfg .. common,
        'cmake --build ' .. ft .. '/build/' .. cfg .. ' --config ' .. cfg,
        'cmake -E copy "' .. ft .. '/build/' .. cfg .. '/' .. cfg .. '/' .. ftLib .. '" "' .. LibLocation .. '"',

        'cmake -S ' .. glfw .. ' -B ' .. glfw .. '/build/' .. cfg .. common,
        'cmake --build ' .. glfw .. '/build/' .. cfg .. ' --config ' .. cfg,
        'cmake -E copy "' .. glfw .. '/build/' .. cfg .. '/src/' .. cfg .. '/glfw3.lib" "' .. LibLocation .. '"',
    }
end
 
    postbuildcommands {
    "{MKDIR} %[" .. LibLocation .. "]",
    "{COPYFILE} %[%{!cfg.buildtarget.abspath}] %[" .. LibLocation .. "]"
    }
    filter{"system:windows","configurations:Debug"}
        prebuildcommands {
            ThirdPartyBuild("Debug","MultiThreadedDebugDLL")
        }
    filter{"system:windows","configurations:Release"}
        prebuildcommands {
            ThirdPartyBuild("Release","MultiThreadedDLL")
        }

    filter{"system:linux"}
     prebuildcommands {
      "cd ../vendor/freetype",
      "mkdir -p build/%[%{!cfg.buildcfg}]",
      'cmake -S . -B build/%[%{!cfg.buildcfg}] -G "Unix Makefiles"',
      'cmake --build build/%[%{!cfg.buildcfg}]',
      'cp build/%[%{!cfg.buildcfg}]/freetype**.so %[' .. LibLocation ..']'
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

