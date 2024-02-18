#pragma once

#include "manager/components/Resource.h"

#include "flecs.h"
#include <string>

flecs::entity create_resource_node_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id);