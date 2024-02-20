#pragma once

#include "flecs.h"

#include "octopus/components/step/Step.hh"

#include "manager/components/Resource.h"

/// @brief component used to trigger init
struct HarvesterInit {};

struct HarvesterStatic {
	int32_t time_for_harvest = 0;
	flecs::ref<ResourceStore> store;
	int32_t range = 0;
	int32_t harvest_qty = 0;
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
