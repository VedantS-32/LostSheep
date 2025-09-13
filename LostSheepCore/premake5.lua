project "LostSheepCore"
	location "%{wks.location}/LostSheepCore"
	kind "ConsoleApp"
	language "C"
	staticruntime "Off"
	flags { "MultiProcessorCompile" }

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{wks.location}/LostSheepCore/Vendor/glad/include/glad/glad.h",
		"%{wks.location}/LostSheepCore/Vendor/glad/src/glad.c",

		"%{wks.location}/LostSheepCore/Vendor/cglm/include/**h",

		"%{wks.location}/LostSheepCore/Vendor/stb_image/stb_image.h",

		"Source/**.h",
		"Source/**.c"
	}

	includedirs {
		"Source",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.cglm}",
		"%{IncludeDir.clay}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.freetype}"
	}

	links {
		"opengl32",
		"User32",
		"shell32",
		"gdi32",
		"glfw",
		"freetype"
	}

	filter "action:vs*"
	postbuildcommands {
		("{COPY} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/* ./")
	}

	filter "action:not vs*"
		postbuildcommands {
			("cp -f %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/* ./")
		}

	filter "action:vs*"
		buildoptions { "/utf-8" }

	filter "system:windows"
		systemversion "latest"
		defines {
			"LSH_PLATFORM_WINDOWS",
			"LSH_PROJECT"
		}

	filter "system:linux"
		defines {
			"LSH_PLATFORM_LINUX",
			"LSH_PROJECT"
		}
		
		filter "system:macosx"
		defines {
			"LSH_PLATFORM_MACOSX",
			"LSH_PROJECT"
		}

	filter "configurations:Debug"
		defines "LSH_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "LSH_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "LSH_DIST"
		runtime "Release"
		optimize "On"
