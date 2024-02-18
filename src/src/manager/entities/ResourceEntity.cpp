#include "ResourceEntity.h"

#include "octopus/components/basic/Position.hh"
#include "manager/components/Display.h"

flecs::entity create_resource_node_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id)
{
	using namespace octopus;

	return ecs.prefab(name_p.c_str())
		.set<DrawInfo>({frame_id})
		.override<Position>()
		.override<Drawable>()
	;
}
