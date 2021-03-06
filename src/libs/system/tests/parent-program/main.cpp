#include "program/program.hpp"
#include "io/std_input_stream.hpp"
#include "io/std_output_stream.hpp"
#include "system/process.hpp"
#include "system/pipe.hpp"
#include <iostream>

class parent_program : public gorc::program {
public:
    std::string prog_to_run;
    std::vector<std::string> extra_args;
    bool no_join = false;
    bool double_join = false;

    virtual void create_options(gorc::options &opts) override
    {
        opts.insert(gorc::make_value_option("run", prog_to_run));
        opts.insert(gorc::make_switch_option("no-join", no_join));
        opts.insert(gorc::make_switch_option("double-join", double_join));
        opts.insert(gorc::make_multi_value_option("extra", std::back_inserter(extra_args)));
        opts.emplace_constraint<gorc::required_option>("run");
    }

    virtual int run() override
    {
        gorc::std_input_stream std_input_stream;
        gorc::std_output_stream std_output_stream;

        std::cout << "= executing " << prog_to_run << std::endl;

        gorc::pipe std_input;
        gorc::pipe std_output;
        gorc::pipe std_error;

        // Prevent this pipe from being closed in the parent process.
        // This avoids a race condition involving processes that quickly close stdin.
        std_input.set_reusable(true);

        gorc::process child(prog_to_run,
                            extra_args,
                            &std_input,
                            &std_output,
                            &std_error);

        std_input_stream.copy_to(std_input.get_output());
        std_input.set_reusable(false);
        std_input.close_output();
        std_input.close_input();

        std::cout << "==== stdout ====" << std::endl;
        std_output.get_input().copy_to(std_output_stream);
        std::cout << std::endl;

        std::cout << "==== stderr ====" << std::endl;
        std_error.get_input().copy_to(std_output_stream);
        std::cout << std::endl;

        if(no_join) {
            std::cout << "= not joining" << std::endl;
        }
        else {
            int child_result = child.join();
            std::cout << "= exit code " << child_result << std::endl;
        }

        if(double_join) {
            std::cout << "= joining again" << std::endl;
            child.join();
        }

        return EXIT_SUCCESS;
    }
};

MAKE_MAIN(parent_program)
