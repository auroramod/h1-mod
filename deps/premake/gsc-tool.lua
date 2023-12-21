gsc_tool = {
    source = path.join(dependencies.basePath, "gsc-tool")
}

function gsc_tool.import()
    links {"xsk-gsc-h1", "xsk-gsc-utils"}
    gsc_tool.includes()
end

function gsc_tool.includes()
    includedirs {
        path.join(gsc_tool.source, "include")
    }
end

function gsc_tool.project()
    project "xsk-gsc-utils"
    kind "StaticLib"
    language "C++"
    warnings "Off"

    files {
        path.join(gsc_tool.source, "include/xsk/utils/*.hpp"), 
        path.join(gsc_tool.source, "src/utils/*.cpp")
    }

    includedirs {
        path.join(gsc_tool.source, "include")
    }

    zlib.includes()

    project "xsk-gsc-h1"
    kind "StaticLib"
    language "C++"
    warnings "Off"

    filter "action:vs*"
        buildoptions "/Zc:__cplusplus"
    filter {}

    files {
        path.join(gsc_tool.source, "include/xsk/stdinc.hpp"),
 
        path.join(gsc_tool.source, "include/xsk/gsc/engine/h1.hpp"),
        path.join(gsc_tool.source, "src/gsc/engine/h1.cpp"),

        path.join(gsc_tool.source, "src/gsc/engine/h1_code.cpp"),
        path.join(gsc_tool.source, "src/gsc/engine/h1_func.cpp"),
        path.join(gsc_tool.source, "src/gsc/engine/h1_meth.cpp"),
        path.join(gsc_tool.source, "src/gsc/engine/h1_token.cpp"), path.join(gsc_tool.source, "src/gsc/*.cpp"),

        path.join(gsc_tool.source, "src/gsc/common/*.cpp"),
        path.join(gsc_tool.source, "include/xsk/gsc/common/*.hpp")
    }

    includedirs {
        path.join(gsc_tool.source, "include")
    }
end

table.insert(dependencies, gsc_tool)
