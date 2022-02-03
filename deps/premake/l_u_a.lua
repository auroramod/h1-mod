-- Scripts or variables named lua fuck with premake ._.
l_u_a = {
	source = path.join(dependencies.basePath, "lua"),
}

function l_u_a.import()
	links { "lua" }
	l_u_a.includes()
end

function l_u_a.includes()
	includedirs {
		l_u_a.source
	}

end

function l_u_a.project()
	project "lua"
		language "C"

		l_u_a.includes()

		files {
			path.join(l_u_a.source, "*.h"),
			path.join(l_u_a.source, "*.c"),
		}

		removefiles {
			path.join(l_u_a.source, "onelua.c"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, l_u_a)
