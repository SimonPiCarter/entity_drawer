#pragma once

#include "library/Library.hh"
#include "step/Step.hh"

#include <list>

std::list<octopus::Steppable *> TestLevel(octopus::Library &lib_p, size_t number_p);

