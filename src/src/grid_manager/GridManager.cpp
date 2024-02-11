#include "GridManager.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include "utils/RandomGenerator.hh"
#include "utils/Vector.hh"

#include <chrono>

namespace godot {


GridManager::~GridManager()
{
	_over = true;
	delete _controllerThread;
}

void GridManager::init(int number_p)
{
	UtilityFunctions::print("init");

	size_t size_l = 512;

	octopus::RandomGenerator gen_l(42);

	::init(_grid, size_l, size_l);

	FrameInfo const & info_l = _framesLibrary->getFrameInfo("test");
	for(size_t i = 0 ; i < 15000; ++ i)
	{
		ent ent_l;
		ent_l.pos.x = gen_l.roll_double(0, size_l-1);
		ent_l.pos.y = gen_l.roll_double(0, size_l-1);
		// ent_l.pos.x = 11+i;
		// ent_l.pos.y = 11+i;

		_entities.push_back(ent_l);
		// spawn unit
		int idx_l = _drawer->add_instance(8*Vector2(octopus::to_double(ent_l.pos.x), octopus::to_double(ent_l.pos.y)),
			info_l.offset, info_l.sprite_frame, "run", "", false);
		_drawer->add_direction_handler(idx_l, info_l.has_up_down);
	}

	_drawer->set_time_step(0.1);

	UtilityFunctions::print("done");
	_init = true;
	delete _controllerThread;
	// _controllerThread = new std::thread(&GridManager::loop, this);
}

void GridManager::loop()
{
	auto start{std::chrono::steady_clock::now()};
	int idx_l = 0;
	size_t step_l = _entities.size() / 12;
	std::vector<std::function<void()>> jobs_l;
	for(size_t i = 0 ; i < 12 ; ++ i)
	{
		size_t s = step_l*i;
		size_t e = step_l*(i+1);
		if(i==11) { e = _entities.size(); }

		jobs_l.push_back(
			[this, s, e]()
			{
				for(size_t i = s ; i < e ; ++ i)
				{
					ent &ent_l = _entities[i];
					octopus::Vector pos_l(ent_l.pos.x, ent_l.pos.y);
					octopus::Vector target_l(256,256);
					octopus::Vector dir_l = target_l - pos_l;
					octopus::Fixed length_l = octopus::length(dir_l);
					if(length_l > 0.1)
					{
						dir_l = (dir_l / length_l);
						ent_l.speed = vec{0.2*octopus::to_double(dir_l.x), 0.2*octopus::to_double(dir_l.y)};
						ent_l.move(_grid);
					}
				}
			}
		);
	}

	enqueue_and_wait(_pool, jobs_l);

    auto end{std::chrono::steady_clock::now()};
    std::chrono::duration<double> elapsed_seconds{end - start};

	UtilityFunctions::print("move seq ", elapsed_seconds.count());
}

void GridManager::_process(double delta)
{
	Node::_process(delta);
	if(!_init)
	{
		return;
	}
	_elapsed += delta;

	if(_elapsed >= 0.1)
	{
		_elapsed -= 0.1;

		// finish old loop
		if(_controllerThread)
		{
			_controllerThread->join();
			int idx_l = 0;
			for(ent &ent_l : _entities)
			{
				_drawer->set_new_pos(idx_l++, 8*Vector2(octopus::to_double(ent_l.pos.x), octopus::to_double(ent_l.pos.y)));
			}
			_drawer->update_pos();
		}

		// new loop
		delete _controllerThread;
		_controllerThread = new std::thread(&GridManager::loop, this);

	}
}

void GridManager::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("init", "number"), &GridManager::init);
	ClassDB::bind_method(D_METHOD("setEntityDrawer", "drawer"), &GridManager::setEntityDrawer);
	ClassDB::bind_method(D_METHOD("getEntityDrawer"), &GridManager::getEntityDrawer);
	ClassDB::bind_method(D_METHOD("setFramesLibrary", "library"), &GridManager::setFramesLibrary);
	ClassDB::bind_method(D_METHOD("getFramesLibrary"), &GridManager::getFramesLibrary);

	ADD_GROUP("GridManager", "GridManager_");
}

void GridManager::setEntityDrawer(EntityDrawer *drawer_p)
{
	_drawer = drawer_p;
}
EntityDrawer *GridManager::getEntityDrawer() const
{
	return _drawer;
}

void GridManager::setFramesLibrary(FramesLibrary *lib_p)
{
	_framesLibrary = lib_p;
}

FramesLibrary *GridManager::getFramesLibrary() const
{
	return _framesLibrary;
}

}
