#pragma once
#include "game/game.hpp"
#include "script_value.hpp"

namespace ui_scripting
{
	class lightuserdata
	{
	public:
		lightuserdata(void*);
		void* ptr;
	};

	class userdata
	{
	public:
		userdata(void*);

		userdata(const userdata& other);
		userdata(userdata&& other) noexcept;

		~userdata();

		userdata& operator=(const userdata& other);
		userdata& operator=(userdata&& other) noexcept;

		script_value get(const script_value& key) const;
		void set(const script_value& key, const script_value& value) const;

		void* ptr;

	private:
		void add();
		void release();

		int ref{};
	};

	class table
	{
	public:
		table();
		table(game::hks::HashTable* ptr_);

		table(const table& other);
		table(table&& other) noexcept;

		~table();

		table& operator=(const table& other);
		table& operator=(table&& other) noexcept;

		script_value get(const script_value& key) const;
		void set(const script_value& key, const script_value& value) const;

		game::hks::HashTable* ptr;

	private:
		void add();
		void release();

		int ref{};
	};

	class function
	{
	public:
		function(game::hks::cclosure*, game::hks::HksObjectType);

		function(const function& other);
		function(function&& other) noexcept;

		~function();

		function& operator=(const function& other);
		function& operator=(function&& other) noexcept;

		arguments call(const arguments& arguments) const;

		game::hks::cclosure* ptr;
		game::hks::HksObjectType type;

	private:
		void add();
		void release();

		int ref{};
	};
}
