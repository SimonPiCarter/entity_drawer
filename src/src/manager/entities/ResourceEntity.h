#pragma once

#include <godot_cpp/variant/utility_functions.hpp>
#include "flecs.h"
#include <string>
#include <vector>


#include "octopus/utils/Grid.hh"

#include "manager/components/Resource.h"

flecs::entity create_resource_node_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id);

std::vector<flecs::entity> get_resource_in_range(octopus::Grid const &grid_p, long long x, long long y, long long range_p, flecs::entity resource_p);
std::vector<flecs::entity> get_resource_available_in_range(octopus::Grid const &grid_p, long long x, long long y, long long range_p, flecs::entity resource_p);
int32_t get_resource_amount_available(octopus::Grid const &grid_p, long long x, long long y, long long range_p, flecs::entity resource_p);
