wintoast = {
	source = path.join(dependencies.basePath, "WinToast"),
}

function wintoast.import()
	links { "WinToast" }
	wintoast.includes()
end

function wintoast.includes()
	includedirs {
		path.join(wintoast.source, "src"),
	}
end

function wintoast.project()
	project "WinToast"
		language "C++"

		wintoast.includes()
		rapidjson.import();

		files {
			path.join(wintoast.source, "src/*.h"),
			path.join(wintoast.source, "src/*.cpp"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, wintoast)
