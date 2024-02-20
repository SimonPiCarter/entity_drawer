#include "HarvesterEntity.h"

#include "octopus/components/basic/Position.hh"
#include "octopus/components/basic/HitPoint.hh"
#include "octopus/components/basic/Team.hh"
#include "octopus/components/generic/Components.hh"

#include "manager/components/Display.h"
#include "manager/entities/ResourceEntity.h"
#include "manager/step/CustomStepContainer.h"

flecs::entity create_harvester_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id)
{
	using namespace octopus;

	return ecs.prefab(name_p.c_str())
		.set_override<HitPoint>({250})
		.override<Position>()
		.override<Team>()
		.override<Drawable>()
		.override<Harvester>()
		.set<DrawInfo>({frame_id});
}

void create_harvester_systems(
	flecs::world &ecs,
	octopus::Grid const &grid_p,
	int32_t const &timestamp_p,
	ThreadPool &pool_p,
	std::vector<CustomStepContainer> &steps_p
)
{
	using namespace octopus;

	ecs.system<HarvesterStatic, Position const>()
		// .multi_threaded()
        .term_at(1).second(flecs::Wildcard) // Change first argument to (HarvesterStatic, *)
		.kind<octopus::Iteration>()
		.with<HarvesterInit>()
		.each([&](flecs::iter& it, size_t index, HarvesterStatic &hs, Position const &pos) {
        	flecs::entity e = it.entity(index);
			flecs::entity type = it.pair(1).second();

			long long x = pos.vec.x.to_int();
			long long y = pos.vec.y.to_int();
			hs.harvest_qty = get_resource_amount_available(grid_p, x, y, hs.range, type);

			std::vector<flecs::entity> resources_l = get_resource_available_in_range(grid_p, x, y, hs.range, type);

			for(flecs::entity &resource_l : resources_l)
			{
				flecs::ref<ResourceNode> ref_l = resource_l.get_ref_second<ResourceNode>(type);
				ResourceNodeStep step_l {*ref_l.try_get()};

				step_l.data.used_by = e;

				steps_p[0].resource_nodes.add_step(ref_l, std::move(step_l));
			}
			e.remove<HarvesterInit>();
		});

	ecs.system<HarvesterStatic const, Harvester const>()
        .term_at(1).second(flecs::Wildcard) // Change first argument to (HarvesterStatic, *)
		.kind<octopus::Iteration>()
		.iter([&](flecs::iter& it, HarvesterStatic const *hs, Harvester const *h) {
			threading(it.count(), pool_p, [&](size_t t, size_t s, size_t e) {
				for (size_t j = s; j < e; j ++) {
					flecs::entity &ent = it.entity(j);
					if(hs[j].time_for_harvest + h[j].timestamp_last_harvest < timestamp_p+1)
					{
						HarvesterStep step;
						step.data.timestamp_last_harvest = timestamp_p;
						steps_p[t].harvesters.add_step(ent, std::move(step));

						ResourceStoreStep step_res;
						step_res.data.amount = hs[j].harvest_qty;
						steps_p[t].resource_stores.add_step(hs[j].store, std::move(step_res));
					}
				}
			}
			);
		});
}
