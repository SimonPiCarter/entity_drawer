#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

#include "drawer/EntityDrawer.h"
#include "grid_manager/GridManager.h"
#include "manager/GameManager.h"
#include "manager/lib/frames/FramesLibrary.h"

void initialize_draw_entity_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  // REGISTER CLASSES HERE LATER
    godot::ClassDB::register_class<godot::EntityDrawer>();
    godot::ClassDB::register_class<godot::FramesLibrary>();
    godot::ClassDB::register_class<godot::GameManager>();
    godot::ClassDB::register_class<godot::GridManager>();
}

void uninitialize_draw_entity_module(godot::ModuleInitializationLevel p_level) {
  if (p_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  // DO NOTHING
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT draw_entity_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_draw_entity_module);
    init_obj.register_terminator(initialize_draw_entity_module);
    init_obj.set_minimum_library_initialization_level(godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
