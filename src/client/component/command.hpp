#pragma once
#include "game/game.hpp"

namespace command
{
	class params
	{
	public:
		params();

		int size() const;
		const char* get(int index) const;
		std::string join(int index) const;
		std::vector<std::string> get_all() const;

		const char* operator[](const int index) const
		{
			return this->get(index); //
		}

	private:
		int nesting_;
	};

	class params_sv
	{
	public:
		params_sv();

		int size() const;
		const char* get(int index) const;
		std::string join(int index) const;
		std::vector<std::string> get_all() const;

		const char* operator[](const int index) const
		{
			return this->get(index); //
		}

	private:
		int nesting_;
	};

	void read_startup_variable(const std::string& dvar);
	void add(const char* name, const std::function<void(const params&)>& callback);
	void add(const char* name, const std::function<void()>& callback);
	void execute(std::string command, bool sync = false);
}
