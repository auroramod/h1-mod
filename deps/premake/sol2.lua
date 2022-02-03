sol2 = {
	source = path.join(dependencies.basePath, "sol2"),
}

function sol2.import()
	sol2.includes()
	l_u_a.import()
end

function sol2.includes()
	includedirs {
		path.join(sol2.source, "include")
	}
end

function sol2.project()

end

table.insert(dependencies, sol2)
