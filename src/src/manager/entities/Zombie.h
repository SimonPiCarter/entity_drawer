#pragma once

#include "octopus/utils/Grid.hh"

#include "octopus/components/basic/Attack.hh"
#include "octopus/components/basic/Position.hh"
#include "octopus/components/basic/HitPoint.hh"
#include "octopus/components/basic/Team.hh"
#include "octopus/components/behaviour/target/Target.hh"
#include "octopus/components/step/StepContainer.hh"

#include "manager/components/SpawnTime.h"

struct Zombie {};

flecs::entity create_zombie_prefab(flecs::world &ecs);

void zombie_routine(
	octopus::StepContainer &step,
    octopus::Grid const &grid_p,
    int32_t timestamp_p,
    flecs::entity e,
    octopus::Position const & p,
    octopus::Speed const & speed,
    octopus::Target const& target,
    octopus::Team const &team,
    octopus::Attack const &a,
    SpawnTime const &spawn_time
);
