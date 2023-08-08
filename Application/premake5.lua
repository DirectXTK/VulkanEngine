project "Application"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

    files{
        "Source/**.cpp",
        "Source/**.h"
    }
    
    links{
      "vulkan-1.lib",
      "glfw3.lib",
      "Renderer"
    }

    includedirs{
      "%{wks.location}/Include",

      "%{wks.location}/Include/vulkan/",
      "%{wks.location}/Renderer/Source/"
    }

    libdirs{
        "%{wks.location}/Libs"
    }

    defines{
        "LINUX"
    }

    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"

    filter{"configurations:Debug"}
      runtime "Release"
      optimize "on"
