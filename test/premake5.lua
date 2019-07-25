project "test"
	language "C++"
	kind "ConsoleApp"

	libdirs { "lib" }

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["Test files"] = "**.json",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.cpp",
		"**.h",
	}

	filter { "system:windows", "platforms:x86" }
		links { "libcrypto.lib" }
	
	filter { "system:windows", "platforms:x64" }
		links { "libcrypto_64.lib" }

	filter "system:linux"
		links { "crypto" }
