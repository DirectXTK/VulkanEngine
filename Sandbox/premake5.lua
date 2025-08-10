project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir("../bin".. outputdir.."/%{prj.name}")
    objdir("../bin-int".. outputdir.."/%{prj.name}")

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
      GlobalIncludes,

    }

    libdirs{
        "%{prj.location}/../Libs/",
        vulkanSDK.."/Lib/",
    }

     
    links{
      "freetype","z","png16","bz2","brotlidec",
      "glfw3",
      "vulkan",
      "Renderer",
      "Application",
    }

    defines{
        "linux"
    }
    postbuildcommands { "echo lafaf","touch ../ExeLocation","echo "..outputdir.."/%{prj.name} > ../ExeLocation" }

    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"


    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"



