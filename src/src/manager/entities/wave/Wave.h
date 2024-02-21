#pragma once

#include "flecs.h"

#include <vector>

#include "octopus/components/basic/Position.hh"
#include "octopus/utils/Grid.hh"

#include "manager/components/Spawner.h"

struct CustomStepContainer;

struct Wave {
	int32_t time_start = 0;
	Spawner spawner;
	std::vector<octopus::Position> pos;
	int32_t spawned = 0;
	int32_t number = 0;
};

void wave_system(
	flecs::world &ecs,
	octopus::Grid const &grid_p,
	int32_t const &timestamp_p,
	std::vector<CustomStepContainer> &steps_p,
	std::vector<Spawner> &spawners_p);
