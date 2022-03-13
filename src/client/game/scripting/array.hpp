#pragma once
#include "game/game.hpp"
#include "script_value.hpp"

namespace scripting
{
	class array_value : public script_value
	{
	public:
		array_value(unsigned int, unsigned int);
		void operator=(const script_value&);
	private:
		unsigned int id_;
		unsigned int parent_id_;
	};

	class array final
	{
	public:
		array();
		array(const unsigned int);

		array(std::vector<script_value>);
		array(std::unordered_map<std::string, script_value>);

		array(const array& other);
		array(array&& other) noexcept;

		~array();

		array& operator=(const array& other);
		array& operator=(array&& other) noexcept;

		std::vector<script_value> get_keys() const;
		int size() const;

		unsigned int push(script_value) const;
		void erase(const unsigned int) const;
		void erase(const std::string&) const;
		script_value pop() const;

		script_value get(const script_value&) const;
		script_value get(const std::string&) const;
		script_value get(const unsigned int) const;

		void set(const script_value&, const script_value&) const;
		void set(const std::string&, const script_value&) const;
		void set(const unsigned int, const script_value&) const;

		unsigned int get_entity_id() const;

		unsigned int get_value_id(const std::string&) const;
		unsigned int get_value_id(const unsigned int) const;

		entity get_raw() const;

		array_value operator[](const int index) const
		{
			return {this->id_, this->get_value_id(index)};
		}

		array_value operator[](const std::string& key) const
		{
			return {this->id_, this->get_value_id(key)};
		}

		template <typename I = int, typename S = std::string>
		array_value operator[](const script_value& key) const
		{
			if (key.is<I>())
			{
				return { this->id_, this->get_value_id(key.as<I>()) };
			}

			if (key.is<S>())
			{
				return { this->id_, this->get_value_id(key.as<S>()) };
			}
		}

	private:
		void add() const;
		void release() const;

		unsigned int id_;
	};
}
