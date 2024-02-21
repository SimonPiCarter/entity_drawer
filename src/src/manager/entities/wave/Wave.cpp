#include "Wave.h"

#include <godot_cpp/variant/utility_functions.hpp>

void wave_system(
	flecs::world &ecs,
	octopus::Grid const &grid_p,
	int32_t const &timestamp_p,
	std::vector<CustomStepContainer> &steps_p,
	std::vector<Spawner> &spawners_p)
{

	using namespace octopus;

	// spawn system
	ecs.system<Wave>()
		.kind<octopus::Iteration>()
		.each([&](flecs::iter& it, size_t idx, Wave &wave_p) {
			for(octopus::Position const &pos_l : wave_p.pos)
			{
				if(wave_p.spawned >= wave_p.number)
				{
					it.entity(idx).destruct();
					break;
				}
				if(is_free(grid_p, pos_l.vec.x.to_int(), pos_l.vec.y.to_int()))
				{
					++wave_p.spawned;
					Spawner spawner_l = wave_p.spawner;
					spawner_l.pos = pos_l;
					spawners_p.push_back(spawner_l);
				}
			}
		});
}
