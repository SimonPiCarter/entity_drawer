#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/Library.hh"
#include "controller/Controller.hh"
#include "step/Step.hh"

#include <thread>
#include "drawer/EntityDrawer.h"
#include "lib/frames/FramesLibrary.h"

namespace godot {

class GameManager : public Node2D {
	GDCLASS(GameManager, Node2D)

public:
	~GameManager();

	/// @brief initialize controller
	void init();

	void loop();

	void _process(double delta_p) override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	void setEntityDrawer(EntityDrawer *drawer_p);
	EntityDrawer *getEntityDrawer() const;

	void setFramesLibrary(FramesLibrary *lib_p);
	FramesLibrary *getFramesLibrary() const;

private:
	octopus::Controller * _controller = nullptr;
	std::thread * _controllerThread = nullptr;

	octopus::Library _lib;
	octopus::State const * _state = nullptr;
	std::list<octopus::StepBundle>::const_iterator _lastIt;
	bool _over = false;
	bool _paused = false;
	bool _init = false;

	EntityDrawer * _drawer = nullptr;
	FramesLibrary * _framesLibrary = nullptr;
};

}
