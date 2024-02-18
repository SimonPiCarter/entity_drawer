
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
//     auto end{std::chrono::steady_clock::now()};
//     std::chrono::duration<double> elapsed_seconds{end - start};

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

//     auto end{std::chrono::steady_clock::now()};
//     std::chrono::duration<double> elapsed_seconds{end - start};

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

int main()
{
	// test_bench();
	// test_ent_move();

	return 0;
}
