protobuf = {
	source = path.join(dependencies.basePath, "protobuf"),
}

function protobuf.import()
	links {
		"protobuf"
	}

	protobuf.includes()
end

function protobuf.includes()
	includedirs {
		path.join(protobuf.source, "src"),
	}
end

function protobuf.project()
	project "protobuf"
		language "C++"

		protobuf.includes()

		files {
			path.join(protobuf.source, "src/**.cc"),
			"./src/**.proto",
		}
		
		removefiles {
			path.join(protobuf.source, "src/**/*test.cc"),
			path.join(protobuf.source, "src/google/protobuf/*test*.cc"),

			path.join(protobuf.source, "src/google/protobuf/testing/**.cc"),
			path.join(protobuf.source, "src/google/protobuf/compiler/**.cc"),

			path.join(protobuf.source, "src/google/protobuf/arena_nc.cc"),
			path.join(protobuf.source, "src/google/protobuf/util/internal/error_listener.cc"),
			path.join(protobuf.source, "**/*_gcc.cc"),
		}
		
		rules {
			"ProtobufCompiler"
		}
		
		defines {
			"_SCL_SECURE_NO_WARNINGS",
			"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS",
			"_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS",
		}
		
		linkoptions {
			"-IGNORE:4221"
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, protobuf)
