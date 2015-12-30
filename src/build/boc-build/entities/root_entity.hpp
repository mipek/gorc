#pragma once

#include "graph_entity.hpp"

namespace gorc {

    class root_entity : public entity {
    private:
        std::unordered_set<entity*> dependencies_value;

    public:
        graph_entity * const graph;

        root_entity(entity_input_stream &);
        root_entity(graph_entity *graph);

        virtual std::string const& name() override;
        virtual std::unordered_set<entity*> const& dependencies() override;

        virtual bool is_dirty() override;
        virtual bool update(gorc::service_registry const &) override;

        virtual void serialize(entity_output_stream &) override;
    };

}
