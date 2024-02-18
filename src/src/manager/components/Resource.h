#pragma once

#include "flecs.h"

#include "octopus/components/step/Step.hh"

struct Wood {};
struct Food {};

struct ResourceStore {
	int32_t amount = 0;
};

struct ResourceStoreStep {
	ResourceStore data;
};

struct ResourceStoreMemento {
	ResourceStore data;

	typedef ResourceStore Data;
	typedef ResourceStoreStep Step;
};

template<>
void octopus::apply_step(ResourceStoreMemento &m, ResourceStoreMemento::Data &d, ResourceStoreMemento::Step const &s);

template<>
void octopus::revert_memento(ResourceStoreMemento::Data &d, ResourceStoreMemento const &memento);

struct ResourceNode {
	int32_t amount = 0;
	// the entity using the resource
	flecs::entity used_by;
};

// routine for applying modification

struct ResourceNodeStep {
	ResourceNode data;
};

struct ResourceNodeMemento {
	ResourceNode data;

	typedef ResourceNode Data;
	typedef ResourceNodeStep Step;
};

template<>
void octopus::apply_step(ResourceNodeMemento &m, ResourceNodeMemento::Data &d, ResourceNodeMemento::Step const &s);

template<>
void octopus::revert_memento(ResourceNodeMemento::Data &d, ResourceNodeMemento const &memento);
