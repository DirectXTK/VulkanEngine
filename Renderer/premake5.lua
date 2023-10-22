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
      "%{wks.location}/Application/Source/",
      "%{wks.location}/Include/",
      "%{wks.location}/Include/vulkan/",

      "%{wks.location}/Include"
    }

     libdirs{
        "%{wks.location}/Libs"
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
