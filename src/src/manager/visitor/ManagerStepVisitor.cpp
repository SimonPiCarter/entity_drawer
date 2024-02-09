#include "ManagerStepVisitor.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include "step/Step.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "step/custom/implem/WindUpStartStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

#include "manager/GameManager.h"

namespace godot
{

void applyVisitor(GameManager &manager_p, octopus::State const &state_p, octopus::Step const &step_p)
{
	ManagerStepVisitor vis_l(manager_p, &state_p);
	// visit intial steps
	for(octopus::Steppable const * steppable_l : step_p.getSteppable())
	{
		vis_l(steppable_l);
	}
	vis_l(&step_p.getProjectileSpawnStep());
	vis_l(&step_p.getProjectileMoveStep());

	if(step_p.getId()+1 == state_p.getStepApplied())
	{
		// update positions
		for(octopus::Entity const *ent_l : state_p.getEntities())
		{
			manager_p.getEntityDrawer()->set_new_pos(ent_l->_handle.index,
				32*Vector2(octopus::to_double(ent_l->_pos.x), octopus::to_double(ent_l->_pos.y)));
		}
		manager_p.getEntityDrawer()->update_pos();
	}
}

void ManagerStepVisitor::visit(octopus::EntityHitPointChangeStep const *steppable_p)
{
	octopus::Entity const * ent_l = _state->getEntity(steppable_p->_handle);
	if(!ent_l->_alive)
	{
		// die unit
		_manager.getEntityDrawer()->remove_direction_handler(steppable_p->_handle.index);
		_manager.getEntityDrawer()->set_animation_one_shot(steppable_p->_handle.index, "death");
	}
}

void ManagerStepVisitor::visit(octopus::UnitSpawnStep const *steppable_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(steppable_p->getHandle());
	FrameInfo const & info_l = _manager.getFramesLibrary()->getFrameInfo(entity_l._model._id);
	// spawn unit
	int idx_l = _manager.getEntityDrawer()->add_instance(32*Vector2(octopus::to_double(entity_l._pos.x), octopus::to_double(entity_l._pos.y)),
		info_l.offset, info_l.sprite_frame, "run", "", false);
	_manager.getEntityDrawer()->add_direction_handler(idx_l);
}

void ManagerStepVisitor::visit(octopus::CustomStep const *steppable_p)
{
	octopus::WindUpStartStep const *windupStart_l = dynamic_cast<octopus::WindUpStartStep const *>(steppable_p);

	if(windupStart_l && _state->isEntityAlive(windupStart_l->_handle))
	{
		_manager.getEntityDrawer()->set_animation(windupStart_l->_handle.index, "slash", "idle");
		octopus::Vector dir_l = _state->getEntity(windupStart_l->_target)->_pos - _state->getEntity(windupStart_l->_handle)->_pos;
		_manager.getEntityDrawer()->set_direction(windupStart_l->_handle.index, 32*Vector2(octopus::to_double(dir_l.x), octopus::to_double(dir_l.y)));
	}
}

} // godot
