#include "GameManager.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include "levels/test/TestLevel.h"
#include "state/State.hh"

#include "visitor/ManagerStepVisitor.h"

namespace godot {


GameManager::~GameManager()
{
	_over = true;
	delete _controller;
	delete _controllerThread;
}

void GameManager::init()
{
	UtilityFunctions::print("init");
	std::list<octopus::Steppable *> spawners_l = TestLevel(_lib, 100);

	delete _controller;
	_controller = new octopus::Controller(spawners_l, 0.01, {}, 5, 50);
	_controller->setExternalMin(0);
	_controller->addQueuedLayer();
	_controller->enableORCA();

	octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
	_state = stateAndSteps_l._state;
	_lastIt = stateAndSteps_l._steps.begin();

	applyVisitor(*this, *_state, stateAndSteps_l._initialStep);

	UtilityFunctions::print("done");
	_init = true;
	delete _controllerThread;
	_controllerThread = new std::thread(&GameManager::loop, this);
}

void GameManager::loop()
{
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;

	UtilityFunctions::print("Playing...");

	// control if step is locked
	try
	{
		while(!_over)
		{
			if(!_paused)
			{
				// update controller
				_controller->update(std::min(0.01, elapsed_l));
			}
			while(!_controller->loop_body()) {}

			auto cur_l = std::chrono::steady_clock::now();
			std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
			elapsed_l = elapsed_seconds_l.count();
			last_l = cur_l;
		}
	}
	catch(const std::exception& e)
	{
		UtilityFunctions::print("catched exception ", e.what());
	}

	UtilityFunctions::print("Over");
}

void GameManager::_process(double delta)
{
	Node::_process(delta);

	if(_controller && _init)
	{
		octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
		_state = stateAndSteps_l._state;

		_over |= _state->isOver();
		_paused |= _state->isOver();

		// Every step missing
		for(auto it_l = _lastIt ; it_l != stateAndSteps_l._stepIt ; ++it_l)
		{
			applyVisitor(*this, *_state, *it_l->_step);
		}
		_lastIt = stateAndSteps_l._stepIt;
		_controller->setExternalMin(_lastIt->_step->getId());
	}
}

void GameManager::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("init"), &GameManager::init);
	ClassDB::bind_method(D_METHOD("setEntityDrawer", "drawer"), &GameManager::setEntityDrawer);
	ClassDB::bind_method(D_METHOD("getEntityDrawer"), &GameManager::getEntityDrawer);
	ClassDB::bind_method(D_METHOD("setFramesLibrary", "library"), &GameManager::setFramesLibrary);
	ClassDB::bind_method(D_METHOD("getFramesLibrary"), &GameManager::getFramesLibrary);

	ADD_GROUP("GameManager", "GameManager_");
}

void GameManager::setEntityDrawer(EntityDrawer *drawer_p)
{
	_drawer = drawer_p;
}
EntityDrawer *GameManager::getEntityDrawer() const
{
	return _drawer;
}

void GameManager::setFramesLibrary(FramesLibrary *lib_p)
{
	_framesLibrary = lib_p;
}

FramesLibrary *GameManager::getFramesLibrary() const
{
	return _framesLibrary;
}

}
