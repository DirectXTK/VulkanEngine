project "Sandbox"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

    files{
        "Source/**.cpp",
        "Source/**.h"
    }
    
    links{
      "vulkan-1.lib",
      "glfw3.lib",
      "Renderer",
      "Application"
    }

    includedirs{
      "%{prj.location}/..//Include",

      "%{prj.location}/../Include/vulkan/",
      "%{prj.location}/../Renderer/Source/",
      "%{prj.location}/../Application/Source/"

    }

    libdirs{
        "%{prj.location}/../Libs"
    }

    defines{
        "WINDOWS"
    }

    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"


    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"
