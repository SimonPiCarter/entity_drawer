
#include <chrono>
#include <iostream>

// #include "octopus/utils/Grid.hh"

// using namespace octopus;

// struct vec
// {
// 	double x = 0;
// 	double y = 0;

// 	vec operator+(vec const &other_p)
// 	{
// 		vec v {x,y};
// 		v.x += other_p.x;
// 		v.y += other_p.y;
// 		return v;
// 	}
// };

// struct ent
// {
// 	vec speed = {0.1,0.};

// 	vec pos = {50, 50};

// 	bool move(Grid &grid_p)
// 	{
// 		vec new_pos = pos + speed;
// 		if(size_t(new_pos.x) == size_t(pos.x) && size_t(new_pos.y) == size_t(pos.y))
// 		{
// 			pos = new_pos;
// 			return true;
// 		}
// 		if(is_free(grid_p, size_t(new_pos.x), size_t(new_pos.y)))
// 		{
// 			set(grid_p, size_t(pos.x), size_t(pos.y), false);
// 			set(grid_p, size_t(new_pos.x), size_t(new_pos.y), true);
// 			pos = new_pos;
// 			return true;
// 		}
// 		return false;
// 	}
// };

// void test_ent_move()
// {
// 	Grid grid_l;
// 	size_t size_l = 2048;
// 	size_t nb_l = 50000;

// 	init(grid_l, size_l, size_l);

// 	std::vector<ent> entities_l;
// 	for(size_t i = 0 ; i < nb_l; ++ i)
// 	{
// 		ent ent_l;
// 		ent_l.pos.x = double(i / size_l);
// 		ent_l.pos.y = double(i - i / size_l);
// 		entities_l.push_back(ent_l);
// 	}

// 	size_t stutter_l = 0;

// 	auto start{std::chrono::steady_clock::now()};
// 	for(size_t i = 0 ; i < 15000 ; ++ i)
// 	{
// 		for(ent &ent_l : entities_l)
// 		{
// 			if(!ent_l.move(grid_l)) stutter_l++;
// 		}
// 	}
//	 auto end{std::chrono::steady_clock::now()};
//	 std::chrono::duration<double> elapsed_seconds{end - start};

// 	std::cout << "move seq " << elapsed_seconds.count() << std::endl;
// 	std::cout<<stutter_l<<std::endl;
// }

// void test_bench()
// {
// 	size_t size_l = 2048;
// 	size_t nb_l = 50000;

// 	auto start{std::chrono::steady_clock::now()};

// 	Grid grid_l;

// 	init(grid_l, size_l, size_l);

//	 auto end{std::chrono::steady_clock::now()};
//	 std::chrono::duration<double> elapsed_seconds{end - start};

// 	std::cout << "init " << elapsed_seconds.count() << std::endl;

// 	start = std::chrono::steady_clock::now();

// 	for(size_t i = 0 ; i < nb_l; ++ i)
// 	{
// 		set(grid_l, i%size_l, i%size_l, true);
// 	}

// 	end = std::chrono::steady_clock::now();

// 	elapsed_seconds = end - start;

// 	std::cout << "set " << nb_l << " : " << elapsed_seconds.count() << std::endl;

// 	start = std::chrono::steady_clock::now();

// 	for(size_t i = 0 ; i < nb_l; ++ i)
// 	{
// 		set(grid_l, i%size_l, i%size_l, false);
// 		set(grid_l, i%size_l+1, i%size_l, true);
// 	}

// 	end = std::chrono::steady_clock::now();

// 	elapsed_seconds = end - start;

// 	std::cout << "move x " << nb_l << " : " << elapsed_seconds.count() << std::endl;

// 	start = std::chrono::steady_clock::now();

// 	for(size_t i = 0 ; i < nb_l; ++ i)
// 	{
// 		set(grid_l, i%size_l+1, i%size_l, false);
// 		set(grid_l, i%size_l+1, i%size_l+1, true);
// 	}

// 	end = std::chrono::steady_clock::now();

// 	elapsed_seconds = end - start;

// 	std::cout << "move y " << nb_l << " : " << elapsed_seconds.count() << std::endl;

// }

#include "flecs.h"

struct Food {};
struct Wood {};

// enum Type {
// 	Food,
// 	Wood
// };
struct Harvest { int qty; };
struct Store { int qty; };

int main()
{
	// test_bench();
	// test_ent_move();

	flecs::world ecs;

	auto ent1 = ecs.entity("e1")
		.set<Harvest, Food>({10})
		.set<Harvest, Wood>({10});

	auto ent2 = ecs.entity("e2")
		.set<Harvest, Wood>({10});

	auto ent3 = ecs.entity("store_food")
		.set<Food, Store>({0})
		.set<Wood, Store>({0});

	ecs.system<Harvest>()
	.term_at(1).second(flecs::Wildcard)
	// Iterate the query with a flecs::iter. This makes it possible to inspect
	// the pair that we are currently matched with.
	.each([&ent3](flecs::iter& it, size_t index, Harvest& harvests) {
		flecs::entity e = it.entity(index);
		flecs::entity type = it.pair(1).second();

		std::cout << e.name() << " harvests "
			<< harvests.qty << " " << type.name() << std::endl;

		bool has = ent3.has_second<Store>(type);
		flecs::entity ent = ent3;
		Store const *s = ent.get_second<Store>(type);
		flecs::ref<Store> ref = ent.get_ref_second<Store>(type);
		std::cout<<(bool)s<<std::endl;
		std::cout<<(bool)ref.try_get()<<std::endl;
		if(has)
			std::cout << ent3.name() << " harvests " << type.name() << std::endl;
		else
			std::cout << ent3.name() << " does not harvest " << type.name() << std::endl;
	});

	ecs.progress();

	ent1.get([](const Harvest& p, const Food &) {		// read lock
		std::cout<<"ok"<<std::endl;
	});


	return 0;
}
