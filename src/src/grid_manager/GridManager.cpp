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

#include "manager/components/Destructible.h"
#include "manager/components/Display.h"
#include "manager/components/Resource.h"
#include "manager/entities/Zombie.h"
#include "manager/entities/Unit.h"
#include "manager/entities/ResourceEntity.h"
#include "manager/entities/HarvesterEntity.h"
#include "manager/entities/wave/Wave.h"

namespace godot {

GridManager::~GridManager()
{
	_over = true;
	delete _controllerThread;
	delete _pool;
}

double GridManager::get_world_size() const
{
	return _world_size;
}

int GridManager::get_grid_build_size() const
{
	return _grid_build_size;
}

flecs::entity GridManager::handle_spawner(Spawner const &spawner)
{
	using namespace octopus;

	if(octopus::get(_grid, spawner.pos.vec.x.to_int(), spawner.pos.vec.y.to_int()))
	{
		return flecs::entity();
	}

	flecs::entity ent_l = ecs.entity()
		.is_a(spawner.prefab)
		.set<Position>(spawner.pos);
	if(ent_l.has<SpawnTime>())
	{
		ent_l.get_mut<SpawnTime>()->spawn_timestamp = _timestamp;
	}
	if(spawner.has_team)
	{
		ent_l.set<Team>(spawner.team);
	}
	octopus::set(_grid, spawner.pos.vec.x.to_int(), spawner.pos.vec.y.to_int(), ent_l);

	if(ent_l.has<DrawInfo>())
	{
		DrawInfo const *draw_info_l = ent_l.get<DrawInfo>();
		FrameInfo const & info_l = _framesLibrary->getFrameInfo(draw_info_l->frame_id);

		// spawn unit
		int idx_l = _drawer->add_instance(
			get_world_size()*Vector2(real_t(to_double(spawner.pos.vec.x)), real_t(to_double(spawner.pos.vec.y))),
			info_l.offset, info_l.sprite_frame, spawner.cur_anim, spawner.next_anim, false);
		if(spawner.has_direction)
		{
			_drawer->add_direction_handler(idx_l, info_l.has_up_down);
		}
		_drawer->add_pickable(idx_l);

		ent_l.set<Drawable>({idx_l});
	}
	spawner.func(ent_l);
	return ent_l;
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
	_custom_steps.clear();
	_custom_steps.resize(_pool->size(), CustomStepContainer());

	size_t size_l = 512;

	RandomGenerator gen_l(42);

	octopus::init(_grid, size_l, size_l);

	flecs::entity zombie_model = create_zombie_prefab(ecs);
	flecs::entity hero_model = create_hero_prefab(ecs);

	flecs::entity tree_model = create_resource_node_prefab(ecs, "tree", "tree");
	tree_model.set_override<Wood, ResourceNode>({1, flecs::entity()});
	tree_model.set_override<Food, ResourceNode>({1, flecs::entity()});

	create_harvester_prefab(ecs, "wood_cutter", "wood_cutter");
	create_harvester_prefab(ecs, "food_harvester", "food_harvester");

	ecs.component<Destructible>();

	std::vector<Spawner> spawners_l;
	spawners_l.reserve(2*number_p);
	for(size_t i = 0 ; i < 0; ++ i)
	{
		Position pos;
		pos.vec.x = gen_l.roll_double(0, double(size_l-1));
		pos.vec.y = gen_l.roll_double(0, double(size_l-1));
		spawners_l.push_back({
			tree_model,
			pos,
			false,
			Team(),
			false,
			"default",
			""
		});
	}

	for(size_t i = 0 ; i < 0; ++ i)
	{
		Position pos;
		pos.vec.x = gen_l.roll_double(0, double(size_l-1));
		pos.vec.y = gen_l.roll_double(0, double(size_l-1));
		spawners_l.push_back({
			zombie_model,
			pos,
			true,
			{int8_t(0)},
			true,
			"spawn",
			"run"
		});
	}

	for(Spawner &spawner_l : spawners_l)
	{
		handle_spawner(spawner_l);
	}

	Spawner wave_spawner_l {
		zombie_model,
		octopus::Position(),
		true,
		{int8_t(0)},
		true,
		"spawn",
		"run",
		[](flecs::entity e){
			e.remove<Zombie>();
			e.add<Unit>();
			Move m;
			m.enabled = true;
			m.aggro = true;
			m.destination.vec = octopus::Vector(200, 200);
			e.set<Move>(m);
		}
	};

	std::vector<octopus::Position> pos_l;
	for(int i = 100; i < 150 ; ++ i)
		for(int j = 100; j < 350 ; ++ j)
			pos_l.push_back({{i,j}});
	ecs.entity("wave")
		.set<Wave>({10,
		wave_spawner_l,
		pos_l,
		0,
		number_p}
	);

	Spawner wave_spawner2_l {
		hero_model,
		octopus::Position(),
		true,
		{int8_t(1)},
		true,
		"run",
		"",
		[](flecs::entity e){
			Move m;
			m.enabled = true;
			m.aggro = true;
			m.destination.vec = octopus::Vector(200, 200);
			e.set<Move>(m);
		}
	};

	std::vector<octopus::Position> pos2_l;
	for(int i = 300; i < 350 ; ++ i)
		for(int j = 100; j < 350 ; ++ j)
			pos2_l.push_back({{i,j}});
	ecs.entity("wave2")
		.set<Wave>({10,
		wave_spawner2_l,
		pos2_l,
		0,
		number_p}
	);

	_player = ecs.entity("player")
		.add<Position>()
		.set<HitPoint>({500000})
		.set<Team>({1});
	_player.set<Wood, ResourceStore>({50});
	_player.set<Food, ResourceStore>({50});

	_drawer->set_time_step(0.1);

	/// ITERATION
	ecs.system<Position const, Target const, Team const, Attack const, SpawnTime const, Speed const>()
		.kind<Iteration>()
		.with<Zombie>()
		.iter([this](flecs::iter& it, Position const *pos, Target const *target, Team const *team, Attack const* attack, SpawnTime const* spawn, Speed const *speed) {
			threading(it.count(), *_pool, [&](size_t t, size_t s, size_t e) {
				// set up memory
				reserve(_steps[t], e-s);
				for (size_t j = s; j < e; j ++) {
					flecs::entity &ent = it.entity(j);
					zombie_routine(_steps[t], _grid, _timestamp, ent, pos[j], speed[j], target[j], team[j], attack[j], spawn[j]);
				}
			}
			);
		});
	ecs.system<Position const, Target const, Team const, Attack const, SpawnTime const, Speed const, Move const>()
		.kind<Iteration>()
		.with<Unit>()
		.iter([this](flecs::iter& it, Position const *pos, Target const *target, Team const *team,
						Attack const* attack, SpawnTime const* spawn, Speed const *speed, Move const *move) {
			threading(it.count(), *_pool, [&](size_t t, size_t s, size_t e) {
				// set up memory
				reserve(_steps[t], e-s);
				for (size_t j = s; j < e; j ++) {
					flecs::entity &ent = it.entity(j);
					unit_routine(_steps[t], _grid, _timestamp, ent, pos[j], speed[j], target[j], team[j], attack[j], move[j], spawn[j]);
				}
			}
			);
		});

	create_harvester_systems(ecs, _grid, _timestamp, *_pool, _custom_steps);

	wave_system(ecs, _grid, _timestamp, _custom_steps, _spawned_entities);

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
				// if entity has destructible we remove it and add Destroyed
				if(e.has<Destructible>())
				{
					e.remove<Destructible>();
					e.add<Destroyed>();
				}
				else
				{
					e.destruct();
				}
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
	declare_apply_system(ecs, _custom_steps, *_pool);

	// Create computation pipeline
	_apply = ecs.pipeline()
		.with(flecs::System)
		.with<Apply>()
		.build();


	/// DISPLAY

	ecs.system<Position const, Speed const, Drawable const>()
		.multi_threaded()
		.kind<Display>()
		.each([this](Position const &pos, Speed const &, Drawable const &drawable) {
			_drawer->set_new_pos(drawable.idx, get_world_size() * Vector2(real_t(octopus::to_double(pos.vec.x)), real_t(octopus::to_double(pos.vec.y))));
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

	for(CustomStepContainer &container_l : _custom_steps)
	{
		clear_container(container_l);
	}

	auto end{std::chrono::steady_clock::now()};
	std::chrono::duration<double> diff = end - start;

	// UtilityFunctions::print("total ", diff.count()*1000.);
}

void GridManager::_process(double delta)
{
	Node::_process(delta);
	if(!_init)
	{
		return;
	}
	if(!_paused)
	{
		_elapsed += delta;
	}

	if(_elapsed >= 0.1)
	{
		_elapsed -= 0.1;

		// finish old loop
		if(_controllerThread)
		{
			_controllerThread->join();
			++_timestamp;

			_drawer->update_pos();

			ecs.set_pipeline(_display);
			ecs.progress();

			std::vector<Spawner> non_spawned_l;
			for(Spawner const &spawner_l : _spawned_entities)
			{
				if(!handle_spawner(spawner_l))
				{
					non_spawned_l.push_back(spawner_l);
				}
			}
			std::swap(non_spawned_l, _spawned_entities);

			for(int idx : _destroyed_entities)
			{
				++_dead;
				// keep static sprite
				_drawer->set_new_pos(idx, _drawer->get_old_pos(idx));
				_drawer->remove_direction_handler(idx);
				_drawer->set_animation_one_shot(idx, "death");
			}
			_destroyed_entities.clear();
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
	ClassDB::bind_method(D_METHOD("get_world_size"), &GridManager::get_world_size);
	ClassDB::bind_method(D_METHOD("get_grid_build_size"), &GridManager::get_grid_build_size);
	ClassDB::bind_method(D_METHOD("is_grid_build_free", "x", "y"), &GridManager::is_grid_build_free);
	ClassDB::bind_method(D_METHOD("set_pause", "paused"), &GridManager::set_pause);

	// DEBUG
	ClassDB::bind_method(D_METHOD("set_player", "x", "y", "b"), &GridManager::set_player);
	ClassDB::bind_method(D_METHOD("spawn_hero", "x", "y"), &GridManager::spawn_hero);
	ClassDB::bind_method(D_METHOD("spawn_wood_cutter", "x", "y"), &GridManager::spawn_wood_cutter);
	ClassDB::bind_method(D_METHOD("spawn_food_harvester", "x", "y"), &GridManager::spawn_food_harvester);
	ClassDB::bind_method(D_METHOD("get_wood"), &GridManager::get_wood);
	ClassDB::bind_method(D_METHOD("get_food"), &GridManager::get_food);
	ClassDB::bind_method(D_METHOD("get_dead"), &GridManager::get_dead);

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

bool GridManager::is_grid_build_free(int world_x_p, int world_y_p)
{
	for(int x = std::max<int>(0, world_x_p * _grid_build_size) ; x < std::min<int>((world_x_p + 1)*_grid_build_size, _grid.x) ; ++ x)
	{
		for(int y = std::max<int>(0, world_y_p * _grid_build_size) ; y < std::min<int>((world_y_p + 1)*_grid_build_size, _grid.y) ; ++ y)
		{
			if(octopus::get(_grid, x, y))
			{
				return false;
			}
		}
	}
	return true;
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

void GridManager::spawn_hero(int x, int y)
{
	if(octopus::get(_grid, x, y)) {return;}

	handle_spawner({
		ecs.lookup("hero_model"),
		{{x, y}},
		true,
		{int8_t(1)},
		true,
		"idle",
		"run"
	});

	flecs::query<octopus::Move> q = ecs.query_builder<octopus::Move>().with<Unit>().build();

	q.each([&](octopus::Move& m) {
		m.enabled = true;
		m.aggro = true;
		m.destination.vec = octopus::Vector(x, y);
	});
}

void GridManager::spawn_wood_cutter(int x, int y)
{
	if(octopus::get(_grid, x, y)) {return;}

	int32_t available_l = get_resource_amount_available(_grid, x, y, 4, ecs.component<Wood>());
	UtilityFunctions::print("wood : ", (int)available_l);

	flecs::entity ent_l = handle_spawner({
		ecs.lookup("wood_cutter"),
		{{x, y}},
		true,
		{int8_t(1)},
		false,
		"idle",
		""
	});
	ent_l.set<HarvesterStatic, Wood>({50, _player.get_ref<Wood, ResourceStore>(), 4});
	ent_l.add<HarvesterInit>();

	flecs::query<octopus::Move> q = ecs.query_builder<octopus::Move>().with<Unit>().build();

	q.each([&](octopus::Move& m) {
		m.enabled = true;
		m.aggro = false;
		m.destination.vec = octopus::Vector(x, y);
	});
}

void GridManager::spawn_food_harvester(int x, int y)
{
	if(octopus::get(_grid, x, y)) {return;}

	int32_t available_l = get_resource_amount_available(_grid, x, y, 4, ecs.component<Food>());
	UtilityFunctions::print("food : ", (int)available_l);

	flecs::entity ent_l = handle_spawner({
		ecs.lookup("food_harvester"),
		{{x, y}},
		true,
		{int8_t(1)},
		false,
		"idle",
		""
	});
	ent_l.set<HarvesterStatic, Food>({50, _player.get_ref<Food, ResourceStore>(), 4});
	ent_l.add<HarvesterInit>();
}

template<typename Res_t>
int get_res(flecs::entity ent)
{
	ResourceStore const *store_l = ent.get<Res_t, ResourceStore>();
	if(store_l)
	{
		return store_l->amount;
	}
	return 0;
}

int GridManager::get_wood()
{
	return get_res<Wood>(_player);
}
int GridManager::get_food()
{
	return get_res<Food>(_player);
}

}
