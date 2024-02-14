#pragma once

#include "utils/Fixed.hh"

struct vec
{
	octopus::Fixed x = 0;
	octopus::Fixed y = 0;

	vec operator+(vec const &other_p)
	{
		vec v {x,y};
		v.x += other_p.x;
		v.y += other_p.y;
		return v;
	}
};

struct ent
{
	vec speed = {0.1,0.};

	vec pos = {50, 50};

	bool attacking = false;
	bool running = false;

	bool apply_speed(Grid const &grid_p, Grid &newGrid_p, vec new_speed_p, bool final_p)
	{
		vec new_pos = pos + new_speed_p;
		long long one_l = octopus::Fixed::OneAsLong();
		long long new_x_l = new_pos.x.data() / one_l;
		long long new_y_l = new_pos.y.data() / one_l;
		long long old_x_l = pos.x.data() / one_l;
		long long old_y_l = pos.y.data() / one_l;

		if(new_x_l == old_x_l && new_y_l == old_y_l)
		{
			pos = new_pos;
			return true;
		}
		else if(is_free(grid_p, new_x_l, new_y_l))
		{
			set(newGrid_p, old_x_l, old_y_l, false);
			set(newGrid_p, new_x_l, new_y_l, true);
			pos = new_pos;
			return true;
		}
		else if(!final_p)
		{
			vec other_speed {new_speed_p.y, -new_speed_p.x};
			return apply_speed(grid_p, newGrid_p, other_speed, true);
		}
		return false;
	}

	bool move(Grid const &grid_p, Grid &newGrid_p)
	{
		return apply_speed(grid_p, newGrid_p, speed, false);
	}
};
