#include "GridManager.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include "octopus/utils/RandomGenerator.hh"
#include "octopus/utils/Vector.hh"
#include "octopus/components/basic/Attack.hh"
#include "octopus/components/basic/Position.hh"
#include "octopus/components/basic/HitPoint.hh"
#include "octopus/components/basic/Team.hh"
#include "octopus/components/behaviour/target/Target.hh"

#include <chrono>
#include <sstream>

#include "manager/entities/Zombie.h"
#include "manager/components/Display.h"

namespace godot {

GridManager::~GridManager()
{
	_over = true;
	delete _controllerThread;
	delete _pool;
}

void GridManager::init(int number_p)
{
	using namespace octopus;

	UtilityFunctions::print("init");

	delete _pool;
	uint32_t nb_threads_l = 12;
	_pool = new ThreadPool(nb_threads_l);
	ecs.set_threads(nb_threads_l);
	_steps.clear();
	_steps.resize(_pool->size(), StepContainer());

	size_t size_l = 512;

	RandomGenerator gen_l(42);

	octopus::init(_grid, size_l, size_l);

	flecs::entity zombie_model = create_zombie_prefab(ecs);

	for(size_t i = 0 ; i < number_p; ++ i)
	{
		std::stringstream ss_l;
		ss_l<<"e"<<i;
		Position pos;
		pos.speed = 0.1;
		pos.vec.x = gen_l.roll_double(0, double(size_l-1));
		pos.vec.y = gen_l.roll_double(0, double(size_l-1));
		flecs::entity ent_l = ecs.entity(ss_l.str().c_str())
			.is_a(zombie_model)
			.set<Position>(pos)
			.set<Team>({int8_t(0)});
		octopus::set(_grid, pos.vec.x.to_int(), pos.vec.y.to_int(), ent_l);

		if(ent_l.has<DrawInfo>())
		{
			DrawInfo const *draw_info_l = ent_l.get<DrawInfo>();
			FrameInfo const & info_l = _framesLibrary->getFrameInfo(draw_info_l->frame_id);

			// spawn unit
			int idx_l = _drawer->add_instance(8*Vector2(real_t(to_double(pos.vec.x)), real_t(to_double(pos.vec.y))),
				info_l.offset, info_l.sprite_frame, "run", "", false);
			_drawer->add_direction_handler(idx_l, info_l.has_up_down);

			ent_l.set<Drawable>({idx_l});
		}
	}

	_player = ecs.entity("player")
		.add<Position>()
		.set<HitPoint>({500000})
		.set<Team>({1});

	_drawer->set_time_step(0.1);

	/// ITERATION
	ecs.system<Position const, Target const, Team const, Attack const>()
		.kind<Iteration>()
		.iter([this](flecs::iter& it, Position const *pos, Target const *target, Team const *team, Attack const* attack) {
			threading(it.count(), *_pool, [&it, &pos, &target, &team, &attack, this](size_t t, size_t s, size_t e) {
				// set up memory
				reserve(_steps[t], e-s);
				for (size_t j = s; j < e; j ++) {
					flecs::entity &ent = it.entity(j);
					zombie_routine(_steps[t], _grid, _timestamp, ent, pos[j], target[j], team[j], attack[j]);
				}
			}
			);
		});

    // destruct entities when hp < 0
    ecs.system<HitPoint const>()
        .multi_threaded()
        .kind<Iteration>()
        .each([this](flecs::entity e, HitPoint const &hp) {
            if(hp.hp <= 0)
            {
                // free grid if necessary
                if(e.has<Position>())
                {
                    const Position * pos_l = e.get<Position>();
                    size_t x = size_t(pos_l->vec.x.to_int());
                    size_t y = size_t(pos_l->vec.y.to_int());
                    octopus::set(_grid, x, y, flecs::entity());
                }

                if(e.has<Drawable>())
                {
                    const Drawable * d_l = e.get<Drawable>();
					std::lock_guard lock_l(_destroyed_entities_mutex);
					_destroyed_entities.push_back(d_l->idx);
				}
                e.destruct();
            }
        });

    // move computation
    ecs.system()
        .kind<Iteration>()
        .iter([this](flecs::iter it) {
			threading(_steps.size(), *_pool, [this](size_t t, size_t, size_t) {
				for (StepPair<PositionMemento> &pair : _steps[t].positions.steps) {
					StepPair<PositionMemento> &pair2 = pair;
					flecs::entity &ent = pair.data.entity();
					// maye have been destroyed
					if(ent)
					{
						Position const * pos = pair.data.try_get();
						position_system(_grid, ent, *pos, pair.step);
					}
				}
			}
			);

        });

	// Create computation pipeline
	_iteration = ecs.pipeline()
		.with(flecs::System)
		.with<Iteration>()
		.build();

	/// APPLY
	declare_apply_system(ecs, _steps, *_pool);

	// Create computation pipeline
	_apply = ecs.pipeline()
		.with(flecs::System)
		.with<Apply>()
		.build();


	/// DISPLAY

	ecs.system<Position const, Drawable const>()
		.multi_threaded()
        .kind<Display>()
        .each([this](Position const &pos, Drawable const &drawable) {
			_drawer->set_new_pos(drawable.idx, 8*Vector2(real_t(octopus::to_double(pos.vec.x)), real_t(octopus::to_double(pos.vec.y))));
		});

	ecs.system<Attack const, Drawable const>()
		.multi_threaded()
        .kind<Display>()
        .each([this](Attack const &attack, Drawable const &drawable) {
			if(_timestamp == attack.data.windup_timestamp+1 && _drawer->get_animation(drawable.idx) != StringName("slash"))
			{
				_drawer->set_animation(drawable.idx, "slash", "run");
			}
		});

	// Create computation pipeline
	_display = ecs.pipeline()
		.with(flecs::System)
		.with<Display>()
		.build();

	UtilityFunctions::print("done");
	_init = true;
	delete _controllerThread;
	// _controllerThread = new std::thread(&GridManager::loop, this);
}

void GridManager::loop()
{
	auto start{std::chrono::steady_clock::now()};

	ecs.set_pipeline(_iteration);
	ecs.progress();

	ecs.set_pipeline(_apply);
	ecs.progress();

	for(octopus::StepContainer &container_l : _steps)
	{
		octopus::clear_container(container_l);
	}

    auto end{std::chrono::steady_clock::now()};
    std::chrono::duration<double> diff = end - start;

	UtilityFunctions::print("total ", diff.count()*1000.);
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
			++_timestamp;

			ecs.set_pipeline(_display);
			ecs.progress();

			for(int idx : _destroyed_entities)
			{
				_drawer->remove_direction_handler(idx);
				_drawer->set_animation_one_shot(idx, "death");
			}
			_destroyed_entities.clear();

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
	flecs::entity ent_l = octopus::get(_grid, x, y);
	if(!b && ent_l == _player)
	{
		octopus::set(_grid, x, y, flecs::entity());
	}

	if(b && !ent_l)
	{
		octopus::set(_grid, x, y, _player);
	}
	octopus::Position * pos_l = _player.mut(ecs).get_mut<octopus::Position>();
	pos_l->vec.x = x;
	pos_l->vec.y = y;
}


}
