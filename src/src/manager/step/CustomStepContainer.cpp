#include "CustomStepContainer.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include "octopus/components/generic/Components.hh"

void declare_apply_system(flecs::world &ecs, std::vector<CustomStepContainer> &container, ThreadPool &pool)
{
	ecs.system()
		.kind<octopus::Apply>()
		.iter([&pool, &container](flecs::iter& it) {
			for(size_t i = 0 ; i < container.size(); ++ i)
			{
				std::vector<std::function<void()>> jobs_l;

				jobs_l.push_back([i, &container]() {
					apply_all(container[i].resource_stores);
				});

				jobs_l.push_back([i, &container]() {
					apply_all(container[i].resource_nodes);
				});

				jobs_l.push_back([i, &container]() {
					apply_all(container[i].harvesters);
				});

				enqueue_and_wait(pool, jobs_l);
			}
		});
}

void declare_revert_system(flecs::world &ecs, std::vector<CustomStepContainer> &container, ThreadPool &pool)
{
	ecs.system()
		.kind<octopus::Revert>()
		.iter([&pool, &container](flecs::iter& it) {
			for(size_t i = 0 ; i < container.size(); ++ i)
			{
				std::vector<std::function<void()>> jobs_l;

				jobs_l.push_back([i, &container]() {
					revert_all(container[i].harvesters);
				});

				jobs_l.push_back([i, &container]() {
					revert_all(container[i].resource_nodes);
				});

				jobs_l.push_back([i, &container]() {
					revert_all(container[i].resource_stores);
				});

				enqueue_and_wait(pool, jobs_l);
			}
		});
}

void clear_container(CustomStepContainer &container)
{
	container.resource_stores.steps.clear();
	container.resource_nodes.steps.clear();
	container.harvesters.steps.clear();
}
