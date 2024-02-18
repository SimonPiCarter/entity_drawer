#pragma once

#include <godot_cpp/variant/utility_functions.hpp>
#include "flecs.h"
#include <string>
#include <vector>


#include "octopus/utils/Grid.hh"

#include "manager/components/Resource.h"

flecs::entity create_resource_node_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id);

template<typename resource_t>
std::vector<flecs::entity> get_resource_in_range(octopus::Grid const &grid_p, long long x, long long y, long long range_p)
{
	std::vector<flecs::entity> resources_l;
	for(long long i = std::max<long long>(0, x - range_p) ; i < x+range_p && i < long long(grid_p.x) ; ++ i)
	{
		for(long long j = std::max<long long>(0, y - range_p) ; j < y+range_p && j < long long(grid_p.y) ; ++ j)
		{
			flecs::entity ent_l = octopus::get(grid_p, i, j);
			if(ent_l && ent_l.has<resource_t, ResourceNode>())
			{
				resources_l.push_back(ent_l);
			}
		}
	}
	return resources_l;
}

template<typename resource_t>
std::vector<flecs::entity> get_resource_available_in_range(octopus::Grid const &grid_p, long long x, long long y, long long range_p)
{
	std::vector<flecs::entity> resources_l;
	for(flecs::entity ent_l : get_resource_in_range<resource_t>(grid_p, x, y, range_p))
	{
		const ResourceNode *res_node_l = ent_l.get<resource_t, ResourceNode>();
		if(res_node_l && !res_node_l->used_by)
		{
			resources_l.push_back(ent_l);
		}
	}
	return resources_l;
}

template<typename resource_t>
int32_t get_resource_amount_available(octopus::Grid const &grid_p, long long x, long long y, long long range_p)
{
	int32_t res_l = 0;
	for(flecs::entity ent_l : get_resource_available_in_range<resource_t>(grid_p, x, y, range_p))
	{
		const ResourceNode *res_node_l = ent_l.get<resource_t, ResourceNode>();
		res_l += res_node_l->amount;
	}
	return res_l;
}
