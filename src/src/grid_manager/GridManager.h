#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include <thread>
#include "drawer/EntityDrawer.h"
#include "manager/lib/frames/FramesLibrary.h"
#include "grid/Grid.h"
#include "grid/Entity.h"
#include "utils/ThreadPool.hh"

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

	Grid _grid;
	Grid _grid_player;
	std::vector<ent> _entities;
	ent _player;

	ThreadPool * _pool = nullptr;

	EntityDrawer * _drawer = nullptr;
	FramesLibrary * _framesLibrary = nullptr;
};

}
