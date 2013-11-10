#include "corpse_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"

void gorc::game::world::level::gameplay::corpse_controller::create_controller_data(int thing_id) {
	return;
}

void gorc::game::world::level::gameplay::corpse_controller::remove_controller_data(int thing_id) {
	return;
}

void gorc::game::world::level::gameplay::corpse_controller::handle_animation_marker(int thing_id, enum flags::key_marker_type marker) {
	if(marker == flags::key_marker_type::Death) {
		// Play death thud
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::CorpseHit);
	}
}
