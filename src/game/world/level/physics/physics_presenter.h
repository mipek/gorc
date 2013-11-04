#pragma once

#include "framework/utility/flag_set.h"
#include "framework/math/vector.h"
#include "contact.h"
#include <unordered_map>
#include <set>
#include <vector>
#include <tuple>

namespace gorc {

namespace cog {
namespace verbs {

class verb_table;
}
}

namespace game {

class components;

namespace world {
namespace level {

class level_model;
class level_presenter;

namespace physics {

class physics_presenter {
private:
	level_presenter& presenter;
	level_model* model;

	std::unordered_multimap<int, int> physics_broadphase_thing_influence;
	std::unordered_multimap<int, int> physics_broadphase_sector_things;
	std::set<int> physics_overlapping_things;
	std::unordered_multimap<int, physics::contact> physics_thing_resting_manifolds;
	std::set<int> physics_thing_closed_set;
	std::vector<int> physics_thing_open_set;
	std::set<std::tuple<int, int>> physics_touched_thing_pairs;
	std::set<std::tuple<int, int>> physics_touched_surface_pairs;

	bool physics_surface_needs_collision_response(int moving_thing_id, int surface_id);
	bool physics_thing_needs_collision_response(int moving_thing_id, int collision_thing_id);

	void physics_calculate_broadphase(double dt);
	void physics_find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id, const vector<3>& vel_dir, int current_thing_id);
	void physics_find_thing_resting_manifolds(const physics::sphere& sphere, const vector<3>& vel_dir, int current_thing_id);
	void physics_thing_step(int thing_id, thing& thing, double dt);

	void update_thing_path_moving(int thing_id, thing& thing, double dt);

	class physics_node_visitor {
	private:
		std::unordered_multimap<int, physics::contact>& resting_manifolds;
		std::set<std::tuple<int, int>>& physics_touched_thing_pairs;
		std::stack<matrix<4>> matrices;
		matrix<4> current_matrix = make_identity_matrix<4>();

	public:
		physics_node_visitor(std::unordered_multimap<int, physics::contact>& resting_manifolds, std::set<std::tuple<int, int>>& physics_touched_thing_pairs);

		inline void push_matrix() {
			matrices.push(current_matrix);
		}

		inline void pop_matrix() {
			current_matrix = matrices.top();
			matrices.pop();
		}

		inline void concatenate_matrix(const matrix<4>& mat) {
			current_matrix = current_matrix * mat;
		}

		void visit_mesh(const content::assets::model& model, int mesh_id);

		bool needs_response;
		int moving_thing_id;
		int visited_thing_id;
		physics::sphere sphere;
	} physics_anim_node_visitor;

public:
	physics_presenter(level_presenter& presenter);

	void start(level_model& model);
	void update(double dt);

	static void register_verbs(cog::verbs::verb_table&, components&);
};

}
}
}
}
}
