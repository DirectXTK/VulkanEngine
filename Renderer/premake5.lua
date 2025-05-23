project "Renderer"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

    files{
        "Source/**.cpp",
        "Source/**.h"
    }

    defines{
        "WINDOWS"
    }

    includedirs{
      "%{prj.location}/../Application/Source/",
      vulkanSDK.."/Include/",


      "%{prj.location}/../Include",
      GlobalIncludes,
    }

     libdirs{
        "%{prj.location}/../Libs",
        vulkanSDK.."/Lib/",
    }

    links{
      "vulkan-1.lib",
      "glfw3.lib"
    }

    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"

    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"
