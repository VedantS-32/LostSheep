require "Script/PremakeVSCode"

workspace "LostSheep"
	architecture "x64"
	startproject "LostSheepCore"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	-- Include directories relative to root directory
	IncludeDir = {}
	IncludeDir["glfw"] = "%{wks.location}/LostSheepCore/Vendor/glfw/include"
	IncludeDir["glad"] = "%{wks.location}/LostSheepCore/Vendor/glad/include"
	IncludeDir["cglm"] = "%{wks.location}/LostSheepCore/Vendor/cglm/include"
	IncludeDir["clay"] = "%{wks.location}/LostSheepCore/Vendor/clay"
	IncludeDir["stb_image"] = "%{wks.location}/LostSheepCore/Vendor/stb_image"

	group "Dependencies"
		include "LostSheepCore/Vendor/glfw"
	group ""

include "LostSheepCore"