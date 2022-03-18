#pragma once
#include <utils/nt.hpp>

class loader final
{
public:
	FARPROC load(const utils::nt::library& library, const std::string& buffer) const;
	FARPROC load_library(const std::string& filename) const;

	void set_import_resolver(const std::function<void*(const std::string&, const std::string&)>& resolver);

private:
	std::function<void*(const std::string&, const std::string&)> import_resolver_;

	static void load_section(const utils::nt::library& target, const utils::nt::library& source,
	                         IMAGE_SECTION_HEADER* section);
	void load_sections(const utils::nt::library& target, const utils::nt::library& source) const;
	void load_imports(const utils::nt::library& target, const utils::nt::library& source) const;
	void load_exception_table(const utils::nt::library& target, const utils::nt::library& source) const;
	void load_tls(const utils::nt::library& target, const utils::nt::library& source) const;
};
