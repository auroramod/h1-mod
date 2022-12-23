json = {
    source = path.join(dependencies.basePath, "json")
}

function json.import()
    json.includes()
end

function json.includes()
    includedirs {path.join(json.source, "single_include/*")}
end

function json.project()

end

table.insert(dependencies, json)
