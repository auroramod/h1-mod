shader_tool = {
    source = path.join(dependencies.basePath, "shader-tool"),
}

function shader_tool.import()
    links {"shader-tool"}
    shader_tool.includes()
end

function shader_tool.includes()
    includedirs {
		path.join(shader_tool.source, "include"),
	}
end

function shader_tool.project()
    project "shader-tool"
		kind "StaticLib"
		language "C++"
        warnings "Off"

		shader_tool.includes()

        files {
			path.join(shader_tool.source, "include/**.hpp"),
			path.join(shader_tool.source, "include/**.cpp"),
		}
        
end

table.insert(dependencies, shader_tool)
