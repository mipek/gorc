#pragma once

#include "math/vector.hpp"
#include "math/color.hpp"
#include "client/world/level_view.hpp"
#include "client/client_renderer_object_factory.hpp"

namespace gorc {

class material;

namespace content {
namespace assets {
class model;
class puppet;
}
}

namespace client {
namespace world {

class pov_mesh_node_visitor {
private:
    client_renderer_object_factory &renderer_object_factory;
    color sector_color;
    level_view& view;
    int saber_draw_node = -1;
    float saber_length = 0.0f;
    float saber_base_radius = 0.0f;
    float saber_tip_radius = 0.0f;
    maybe<asset_ref<material>> saber_blade;
    maybe<asset_ref<material>> saber_tip;

public:
    pov_mesh_node_visitor(client_renderer_object_factory &renderer_object_factory, const color& sector_color, level_view& view, int saber_draw_node = -1,
            float saber_length = 0.0f, float saber_base_radius = 0.0f, float saber_tip_radius = 0.0f,
            maybe<asset_ref<material>> saber_blade = nothing, maybe<asset_ref<material>> saber_tip = nothing);

    inline void concatenate_matrix(const matrix<4>& mat) {
        view.concatenate_matrix(mat);
        view.update_shader_model_matrix();
    }

    inline void push_matrix() {
        view.push_matrix();
    }

    inline void pop_matrix() {
        view.pop_matrix();
        view.update_shader_model_matrix();
    }

    void visit_mesh(asset_ref<content::assets::model> model, int mesh_id, int node_id);
};

}
}
}
