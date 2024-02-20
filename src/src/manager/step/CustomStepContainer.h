#pragma once

#include <vector>
#include "octopus/components/step/Step.hh"
#include "octopus/utils/ThreadPool.hh"

#include "manager/components/Resource.h"
#include "manager/components/Harvester.h"


struct CustomStepContainer
{
	octopus::StepVector<ResourceStoreMemento> resource_stores;
	octopus::StepVector<ResourceNodeMemento> resource_nodes;
	octopus::StepVector<HarvesterMemento> harvesters;
};

void declare_apply_system(flecs::world &ecs, std::vector<CustomStepContainer> &container_p, ThreadPool &pool);
void declare_revert_system(flecs::world &ecs, std::vector<CustomStepContainer> &container_p, ThreadPool &pool);

void clear_container(CustomStepContainer &container);
