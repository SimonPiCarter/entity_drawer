#include "flecs.h"

#include <string>
#include <vector>

#include "octopus/utils/ThreadPool.hh"

#include "manager/components/Harvester.h"

struct CustomStepContainer;

flecs::entity create_harvester_prefab(flecs::world &ecs, std::string const &name_p, std::string const &frame_id, HarvesterStatic const &static_p);

void create_harvester_system(flecs::world &ecs, int32_t const &timestamp_p, ThreadPool &pool_p, std::vector<CustomStepContainer> &steps_p);
