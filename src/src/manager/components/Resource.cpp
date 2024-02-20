#include "Resource.h"


template<>
void octopus::apply_step(ResourceStoreMemento &m, ResourceStoreMemento::Data &d, ResourceStoreMemento::Step const &s)
{
	m.data = d;
	d.amount += s.data.amount;
}

template<>
void octopus::revert_memento(ResourceStoreMemento::Data &d, ResourceStoreMemento const &memento)
{
	d = memento.data;
}

template<>
void octopus::apply_step(ResourceNodeMemento &m, ResourceNodeMemento::Data &d, ResourceNodeMemento::Step const &s)
{
	std::swap(d, m.data);
	d = s.data;
}

template<>
void octopus::revert_memento(ResourceNodeMemento::Data &d, ResourceNodeMemento const &memento)
{
	d = memento.data;
}
