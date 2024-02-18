#include "HarvesterEntity.h"

#include "octopus/components/basic/Position.hh"
#include "octopus/components/basic/HitPoint.hh"
#include "octopus/components/basic/Team.hh"
#include "octopus/components/generic/Components.hh"

#include "manager/components/Display.h"
#include "manager/step/CustomStepContainer.h"

flecs::entity create_harvester_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id, HarvesterStatic const &static_p)
{
	using namespace octopus;

	return ecs.prefab(name_p.c_str())
		.set_override<HitPoint>({250})
		.override<Position>()
		.override<Team>()
		.override<Drawable>()
		.override<Harvester>()
		.set<HarvesterStatic>(static_p)
		.set<DrawInfo>({frame_id});
}

void create_harvester_system(flecs::world &ecs, int32_t const &timestamp_p, ThreadPool &pool_p, std::vector<CustomStepContainer> &steps_p)
{
	ecs.system<HarvesterStatic const, Harvester const>()
		.kind<octopus::Iteration>()
		.iter([&](flecs::iter& it, HarvesterStatic const *hs, Harvester const *h) {
			threading(it.count(), pool_p, [&](size_t t, size_t s, size_t e) {
				for (size_t j = s; j < e; j ++) {
					flecs::entity &ent = it.entity(j);
					if(hs[j].time_for_harvest + h[j].timestamp_last_harvest > timestamp_p)
					{
						HarvesterStep step;
						step.data.timestamp_last_harvest = timestamp_p;
						steps_p[t].harvesters.add_step(ent, std::move(step));
					}
				}
			}
			);
		});
}
