#include "Harvester.h"

template<>
void octopus::apply_step(HarvesterMemento &m, HarvesterMemento::Data &d, HarvesterMemento::Step const &s)
{
	std::swap(d, m.data);
	d = s.data;
}

template<>
void octopus::revert_memento(HarvesterMemento::Data &d, HarvesterMemento const &memento)
{
	d = memento.data;
}
