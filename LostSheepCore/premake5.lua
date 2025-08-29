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

		"Source/**.h",
		"Source/**.c"
	}

	includedirs {
		"Source",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.clay}"
	}

	links {
		"opengl32",
		"gdi32",
		"glfw"
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
			"LSH_PLATFORM_WINDOWS"
		}

	filter "system:linux"
		defines {
			"LSH_PLATFORM_LINUX"
		}

	filter "system:macosx"
		defines {
			"LSH_PLATFORM_MACOSX"
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
