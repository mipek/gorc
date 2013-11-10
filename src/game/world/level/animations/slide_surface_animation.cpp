#include "slide_surface_animation.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"

using namespace gorc::math;

gorc::game::world::level::animations::slide_surface_animation::slide_surface_animation(level_model& model, unsigned int surface,
		const vector<3>& direction, int anim_num)
	: model(model), surface(surface), direction(direction) {
	auto& surf = model.surfaces[surface];
	model.surfaces[surface].surface_anim = make_maybe(this);

	// Compute texture basis.
	auto dnsb0 = model.level.vertices[std::get<0>(surf.vertices[1])] - model.level.vertices[std::get<0>(surf.vertices[0])];

	sb0 = dnsb0 / length_squared(dnsb0);
	sb1 = cross(surf.normal, sb0);

	unsigned int noncol_vert;
	for(noncol_vert = 2; noncol_vert < surf.vertices.size(); ++noncol_vert) {
		auto sb2 = model.level.vertices[std::get<0>(surf.vertices[noncol_vert])] - model.level.vertices[std::get<0>(surf.vertices[0])];
		if(dot(sb1, sb2) != 0.0f) {
			break;
		}
	}

	auto vb0 = model.level.texture_vertices[std::get<1>(surf.vertices[1])] - model.level.texture_vertices[std::get<1>(surf.vertices[0])];
	auto vb1 = make_vector(get<1>(vb0), -get<0>(vb0));

	float sgn = 1.0f;
	if(dot(vb1, model.level.texture_vertices[std::get<1>(surf.vertices[noncol_vert])] - model.level.texture_vertices[std::get<1>(surf.vertices[0])]) < 0.0f) {
		sgn = -1.0f;
	}

	tb0 = -vb0;
	tb1 = -sgn * vb1;

	return;
}

void gorc::game::world::level::animations::slide_surface_animation::update(double dt) {
	auto& surf = model.surfaces[surface];
	surf.thrust = direction * rate_factor;

	auto plane_dir = tb0 * dot(direction, sb0) + tb1 * dot(direction, sb1);

	surf.texture_offset += plane_dir * dt * rate_factor;
}

void gorc::game::world::level::animations::slide_surface_animation::stop() {
	auto& surf = model.surfaces[surface];
	model.surfaces[surface].surface_anim = maybe<animation*>();
	surf.thrust = make_zero_vector<3, float>();
}
