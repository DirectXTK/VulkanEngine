project "Renderer"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

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

    filter{"configurations:Debug"}
      runtime "Release"
      optimize "on"
