#pragma once
#include "game/game.hpp"
#include "script_value.hpp"

namespace scripting
{
	class entity final
	{
	public:
		entity();
		entity(unsigned int entity_id);
		entity(game::scr_entref_t entref);

		entity(const entity& other);
		entity(entity&& other) noexcept;

		~entity();

		entity& operator=(const entity& other);
		entity& operator=(entity&& other) noexcept;

		void set(const std::string& field, const script_value& value) const;

		template <typename T = script_value>
		T get(const std::string& field) const;

		script_value call(const std::string& name, const std::vector<script_value>& arguments = {}) const;

		unsigned int get_entity_id() const;
		game::scr_entref_t get_entity_reference() const;

		bool operator ==(const entity& other) const noexcept;
		bool operator !=(const entity& other) const noexcept;

	private:
		unsigned int entity_id_;

		void add() const;
		void release() const;
	};

	template <>
	script_value entity::get(const std::string& field) const;

	template <typename T>
	T entity::get(const std::string& field) const
	{
		return this->get<script_value>(field).as<T>();
	}
}
