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
      "%{wks.location}/Include",

      "%{wks.location}/Include/vulkan/",
      "%{wks.location}/Renderer/Source/",
      "%{wks.location}/Include/",
      "%{wks.location}/Application/Source/"

    }

    libdirs{
        "%{wks.location}/Libs"
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