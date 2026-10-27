project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")
    debugdir "%{wks.location}/"

    TargetDirVar= "../bin".. outputdir.."/%{prj.name}"

    files{
        "Source/**.cpp",
        "Source/**.h",
        "Examples/**.cpp",
        "Examples/**.h",
    }
   

    includedirs{
      "%{prj.location}/..//Include",

      "%{prj.location}/../Include/vulkan/",
      "%{prj.location}/../Renderer/Source/",
      "%{prj.location}/../Application/Source/",
      "%{prj.location}/../vendor/glfw/include",

      GlobalIncludes,

    }

      libdirs{
        "%{prj.location}/../Libs/",
        vulkanSDK.."/Lib/",
    }

    filter{"configurations:Debug"} 
      links{
      "freetyped",
      "glfw3",
      "vulkan-1",
      "Renderer",
      "Application",

      "SPIRVd","glslangd" , "MachineIndependentd" ,  "GenericCodeGend",
        "OSDependentd",
        "OGLCompilerd",         

        "SPIRV-Tools-optd",
        "SPIRV-Toolsd",
        "SPIRV-Tools-linkd",
        "kernel32", "user32", "gdi32", "shell32", "advapi32", "ole32",
       }
    filter{"configurations:Release"}
         links{
      "freetype",
      "glfw3",
      "vulkan-1",
      "Renderer",
      "Application",

      "glslang","SPIRV"  ,"OSDependent", "SPIRV-Tools-opt",
        "SPIRV-Tools",
        "SPIRV-Tools-link",
        }
   

    defines{
    }
    filter{"system:linux"}
        postbuildcommands { "echo lafaf","touch ../ExeLocation","echo "..outputdir.."/%{prj.name} > ../ExeLocation" }
    filter{"system:linux"}
        postbuildcommands {
            'echo %{cfg.targetdir}/%{prj.name}.exe > ../ExeLocation'}
  

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




