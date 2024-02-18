#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include <thread>
#include <vector>
#include <mutex>
#include "drawer/EntityDrawer.h"
#include "manager/lib/frames/FramesLibrary.h"
#include "manager/step/CustomStepContainer.h"

#include "octopus/components/step/StepContainer.hh"
#include "octopus/utils/Grid.hh"

namespace godot {

class GridManager : public Node2D {
	GDCLASS(GridManager, Node2D)

public:
	~GridManager();

	/// @brief initialize controller
	void init(int number_p);

	void loop();

	void _process(double delta_p) override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	void setEntityDrawer(EntityDrawer *drawer_p);
	EntityDrawer *getEntityDrawer() const;

	void setFramesLibrary(FramesLibrary *lib_p);
	FramesLibrary *getFramesLibrary() const;

	// TEST/DEBUG method
	void set_player(int x, int y, bool b);

private:
	std::thread * _controllerThread = nullptr;

	bool _over = false;
	bool _paused = false;
	bool _init = false;
	double _elapsed = 0.;

	octopus::Grid _grid;
	flecs::world ecs;
	flecs::entity _player;
    int32_t _timestamp = 0;
	std::vector<octopus::StepContainer> _steps;
	std::vector<CustomStepContainer> _custom_steps;

	std::vector<int> _destroyed_entities;
	std::mutex _destroyed_entities_mutex;

	// pipelines
	flecs::entity _iteration;
	flecs::entity _apply;
	flecs::entity _display;

	ThreadPool *_pool = nullptr;

	EntityDrawer * _drawer = nullptr;
	FramesLibrary * _framesLibrary = nullptr;
};

}
