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
	delete _pool;
}

void GridManager::init(int number_p)
{
	UtilityFunctions::print("init");

	size_t size_l = 512;

	octopus::RandomGenerator gen_l(42);

	::init(_grid, size_l, size_l);
	::init(_grid_player, size_l, size_l);

	FrameInfo const & info_l = _framesLibrary->getFrameInfo("test");
	for(size_t i = 0 ; i < number_p; ++ i)
	{
		ent ent_l;
		ent_l.pos.x = gen_l.roll_double(0, size_l-1);
		ent_l.pos.y = gen_l.roll_double(0, size_l-1);

		long long one_l = octopus::Fixed::OneAsLong();
		long long pos_x_l = ent_l.pos.x.data() / one_l;
		long long pos_y_l = ent_l.pos.y.data() / one_l;
		::set(_grid, pos_x_l, pos_y_l, true);

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

	Grid newGrid_l;
	::init(newGrid_l, _grid.x, _grid.y);

	int idx_l = 0;
	size_t step_l = _entities.size() / 12;
	std::vector<std::function<void()>> jobs_l;
	for(size_t i = 0 ; i < 12 ; ++ i)
	{
		size_t s = step_l*i;
		size_t e = step_l*(i+1);
		if(i==11) { e = _entities.size(); }

		jobs_l.push_back(
			[this, s, e, &newGrid_l]()
			{
				for(size_t i = s ; i < e ; ++ i)
				{
					ent &ent_l = _entities[i];

					bool move_l = false;
					long long target_x_l = 0;
					long long target_y_l = 0;
					long long closest_l = 0;
					long long one_l = octopus::Fixed::OneAsLong();
					long long pos_x_l = ent_l.pos.x.data() / one_l;
					long long pos_y_l = ent_l.pos.y.data() / one_l;
					::set(newGrid_l, pos_x_l, pos_y_l, true);
					long long r = 6;
					for(long long x = std::max<long long>(0,pos_x_l-r) ; x < std::min<long long>(_grid.x-1,pos_x_l+r) ; ++ x)
					{
						for(long long y = std::max<long long>(0,pos_y_l-r) ; y < std::min<long long>(_grid.y-1,pos_y_l+r) ; ++ y)
						{
							if(!is_free(_grid_player, x, y))
							{
								long long dist_l = std::abs(x-pos_x_l) + std::abs(y-pos_y_l);
								if(!move_l || dist_l < closest_l)
								{
									target_x_l = x;
									target_y_l = y;
									closest_l = dist_l;

									move_l = true;
								}
							}
						}
					}

					if(move_l && closest_l <= 2)
					{
						ent_l.attacking = true;
						move_l = false;
					}

					if(ent_l.running && !move_l)
					{
						move_l = true;
						target_x_l = _player.pos.x.data() / one_l;
						target_y_l = _player.pos.y.data() / one_l;
					}

					if(!move_l)
					{
						continue;
					}
					ent_l.running = true;

					octopus::Vector pos_l(ent_l.pos.x, ent_l.pos.y);
					octopus::Vector target_l(target_x_l,target_y_l);
					octopus::Vector dir_l = target_l - pos_l;
					octopus::Fixed length_l = octopus::length(dir_l);
					if(length_l > 0.1)
					{
						dir_l = (dir_l / length_l);
						ent_l.speed = vec{0.2*octopus::to_double(dir_l.x), 0.2*octopus::to_double(dir_l.y)};
						ent_l.move(_grid ,newGrid_l);
					}
				}
			}
		);
	}

	if(!_pool)
	{
		_pool = new ThreadPool(12);
	}
	enqueue_and_wait(*_pool, jobs_l);

	std::swap(_grid, newGrid_l);

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
				if(ent_l.attacking && _drawer->get_animation(idx_l) != StringName("slash"))
				{
					_drawer->set_animation(idx_l, "slash", "run");
					ent_l.attacking = false;
				}
				_drawer->set_new_pos(idx_l, 8*Vector2(octopus::to_double(ent_l.pos.x), octopus::to_double(ent_l.pos.y)));
				++idx_l;
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

	// DEBUG
	ClassDB::bind_method(D_METHOD("set_player", "x", "y", "b"), &GridManager::set_player);

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


// TEST/DEBUG method
void GridManager::set_player(int x, int y, bool b)
{
	::set(_grid_player, x,y,b);
	_player.pos.x = x;
	_player.pos.y = y;
}


}
