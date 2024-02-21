#pragma once

#include <functional>
#include <godot_cpp/variant/string_name.hpp>
#include "flecs.h"

#include "octopus/components/basic/Position.hh"
#include "octopus/components/basic/Team.hh"

void no_op(flecs::entity);

struct Spawner
{
	flecs::entity prefab;
	octopus::Position pos;
	bool has_team = false;
	octopus::Team team;
	bool has_direction = false;
	godot::StringName cur_anim;
	godot::StringName next_anim;
	std::function<void(flecs::entity)> func = no_op;
};
