gsl = {
	source = path.join(dependencies.basePath, "GSL"),
}

function gsl.import()
	gsl.includes()
end

function gsl.includes()
	includedirs {
		path.join(gsl.source, "include")
	}
end

function gsl.project()

end

table.insert(dependencies, gsl)
