#pragma once


// #include "controller/trigger/Listener.hh"
// #include "command/entity/EntityAttackMoveCommand.hh"
// #include "command/spawner/AreaSpawnerCommand.hh"
// #include "state/entity/Building.hh"
// #include "state/entity/Unit.hh"
// #include "state/entity/Resource.hh"
// #include "state/model/entity/BuildingModel.hh"
// #include "state/vision/PatternHandler.hh"
// #include "state/State.hh"
// #include "step/command/CommandQueueStep.hh"
// #include "step/entity/spawn/UnitSpawnStep.hh"
// #include "step/entity/spawn/ResourceSpawnStep.hh"
// #include "step/entity/spawn/BuildingSpawnStep.hh"
// #include "step/player/PlayerSpawnStep.hh"
// #include "step/player/PlayerSpendResourceStep.hh"
// #include "step/state/StateWinStep.hh"
// #include "step/trigger/TriggerSpawn.hh"
// #include "step/team/TeamVisionStep.hh"
// #include "utils/Binary.hh"

#include "library/Library.hh"
#include "step/Step.hh"

#include <list>

std::list<octopus::Steppable *> TestLevel(octopus::Library &lib_p, size_t number_p);

