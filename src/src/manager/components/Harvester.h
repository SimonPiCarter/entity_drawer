#pragma once

#include "flecs.h"

#include "octopus/components/step/Step.hh"

#include "manager/components/Resource.h"

struct HarvesterStatic {
	int32_t time_for_harvest;
	flecs::ref<ResourceStore> store;
};

struct Harvester {
	int32_t timestamp_last_harvest = 0;
};

struct HarvesterStep {
	Harvester data;
};

struct HarvesterMemento {
	Harvester data;

	typedef Harvester Data;
	typedef HarvesterStep Step;
};

template<>
void octopus::apply_step(HarvesterMemento &m, HarvesterMemento::Data &d, HarvesterMemento::Step const &s);

template<>
void octopus::revert_memento(HarvesterMemento::Data &d, HarvesterMemento const &memento);
