#pragma once

#include "io/input_stream.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "jk/cog/script/constant_table.hpp"
#include "jk/cog/script/script.hpp"
#include "jk/cog/ast/ast.hpp"
#include <memory>

namespace gorc {
    namespace cog {

        class compiler {
        protected:
            verb_table &verbs;
            constant_table &constants;

        public:
            compiler(verb_table &verbs,
                     constant_table &constants);

            std::unique_ptr<script> compile(input_stream &);

            virtual bool handle_parsed_ast(ast::translation_unit &);
            virtual bool handle_analyzed_ast(ast::translation_unit &, script &);
            virtual bool handle_generated_code(script &);
        };

    }
}
