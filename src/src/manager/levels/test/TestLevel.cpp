#include "TestLevel.h"

#include "command/entity/EntityAttackMoveCommand.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/UnitModel.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<Steppable *> TestLevel(Library &lib_p, size_t number_p)
{
	// test
	UnitModel unitModel_l { false, 0.5, 0.045, 50. };
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	unitModel_l._productionTime = 2000;
	unitModel_l._cost["bloc"] = 50;
	unitModel_l._cost["ether"] = 0;
	unitModel_l._cost["steel"] = 0;
	unitModel_l._cost["irium"] = 0;
	unitModel_l._damage = 50;
	unitModel_l._armor = 0;
	unitModel_l._range = 0.2;
	unitModel_l._lineOfSight = 5;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 10;
	unitModel_l._defaultAttackMod = AoEModifier(0.5, 2., false);

	lib_p.registerUnitModel("test", unitModel_l);

	// test2
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	unitModel_l._productionTime = 2000;
	unitModel_l._cost["bloc"] = 50;
	unitModel_l._cost["ether"] = 0;
	unitModel_l._cost["steel"] = 0;
	unitModel_l._cost["irium"] = 0;
	unitModel_l._damage = 5;
	unitModel_l._armor = 0;
	unitModel_l._range = 3.;
	unitModel_l._lineOfSight = 5;
	unitModel_l._fullReload = 100;
	unitModel_l._windup = 10;
	unitModel_l._defaultAttackMod = NoModifier();
	lib_p.registerUnitModel("test2", unitModel_l);

	Unit test_l({ 2, 10. }, false, lib_p.getUnitModel("test"));
	test_l._player = 0;
	Unit test2_l({ 22, 10. }, false, lib_p.getUnitModel("test2"));
	test2_l._player = 1;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
	};

	unsigned long id_l = 0;
	for(size_t i = 0; i < number_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(Handle(id_l++), test_l));
		Handle h_l = Handle(id_l++);
		spawners_l.push_back(new UnitSpawnStep(h_l, test2_l));
		spawners_l.push_back(new CommandSpawnStep(new EntityAttackMoveCommand(h_l, h_l, test_l._pos, 0, {test_l._pos}, true )));
	}

	return spawners_l;
}
