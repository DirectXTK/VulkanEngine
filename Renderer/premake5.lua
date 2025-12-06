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
        "linux"
    }

    includedirs{
      "%{prj.location}/../Application/Source/",
      "%{prj.location}/Source/",
      vulkanSDK.."/Include/",
      "%{prj.location}/../Include",
      GlobalIncludes,
    }

     libdirs{
        "%{prj.location}/../Libs/",
        vulkanSDK.."/Lib/",
    }

    links{
      "vulkan",
      "glfw3",
      "freetype","z","png16","glslang","SPIRV","OGLCompiler"
    }
    postbuildcommands { 
      "{COPY} %{cfg.targetdir}/libRenderer.a "..LibLocation..""
    }


    filter{"configurations:Debug"}
      runtime "Debug"
      symbols "on"
      staticruntime "off"

    filter{"configurations:Release"}
      runtime "Release"
      optimize "on"
