#include "Unit.h"

#include "manager/components/Display.h"

flecs::entity create_hero_prefab(flecs::world &ecs)
{
	using namespace octopus;

	return ecs.prefab("hero_model")
		.set_override<Target>({flecs::entity(), 9})
		.set_override<HitPoint>({50})
		.override<Position>()
		.override<Attack>()
		.override<Team>()
		.override<Drawable>()
		.set_override<SpawnTime>({0,0})
		.override<Move>()
		.set_override<Speed>({0.25})
		.set<DrawInfo>({"hero"})
		.add<Unit>();
}

void unit_routine(
	octopus::StepContainer &step,
	octopus::Grid const &grid_p,
	int32_t timestamp_p,
	flecs::entity e,
	octopus::Position const & p,
	octopus::Speed const & speed,
	octopus::Target const& target,
	octopus::Team const &team,
	octopus::Attack const &a,
	octopus::Move const &move,
	SpawnTime const &spawn_time
)
{
	// let spawn time happen
	if(timestamp_p < spawn_time.spawn_timestamp + spawn_time.spawn_time)
	{
		return;
	}

	bool target_found = false;
	bool do_move = false;
	octopus::Vector diff;
	if(!move.enabled || move.aggro)
	{
		// aquire target
		octopus::target_system(step, grid_p, e, p, target, team);

		if(target.data.target)
		{
			target_found = true;
			octopus::Position const *target_pos = target.data.target.get<octopus::Position>();
			if(target_pos)
			{
				diff = target_pos->vec - p.vec;
				/// @todo use range and size of entity
				bool in_range = square_length(diff) < 2;
				// if in range or already started prepare attack
				if((a.state == octopus::AttackState::Idle && in_range)
				|| a.state != octopus::AttackState::Idle)
				{
					octopus::attack_system(step, timestamp_p, e, a);
				}

				// if we just ended wind up
				if(timestamp_p == a.data.winddown_timestamp+1)
				{
					// deal damage
					octopus::HitPoint const *hp_target = target.data.target.get<octopus::HitPoint>();
					step.hitpoints.add_step(target.data.target, octopus::HitPointStep {-10});
				}

				// if not in wind up/down
				if(a.state == octopus::AttackState::Idle && !in_range)
				{
					do_move = true;
				}
			}
		}
	}

	if(move.enabled && !target_found)
	{
		diff = move.destination.vec - p.vec;
		do_move = square_length(diff) > 1;
	}

	if(do_move)
	{
		diff /= length(diff);
		diff *= speed.speed;
		step.positions.add_step(e, octopus::PositionStep {diff});
	}
}
