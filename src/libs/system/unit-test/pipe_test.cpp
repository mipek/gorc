#include "test/test.hpp"
#include "system/pipe.hpp"
#include <pthread.h>
#include <signal.h>
#include <system_error>

using namespace gorc;

begin_suite(pipe_test);

test_case(simple_pipe_test)
{
    gorc::pipe p;

    std::string out_msg = "Hello, World!";
    p.get_output().write(out_msg.data(), out_msg.size());

    std::string in_msg;
    in_msg.resize(out_msg.size());
    p.get_input().read(&in_msg[0], in_msg.size());

    assert_eq(in_msg, out_msg);
}

test_case(broken_pipe_test)
{
    ::sigset_t set;
    ::sigemptyset(&set);
    ::sigaddset(&set, SIGPIPE);
    ::pthread_sigmask(SIG_BLOCK, &set, nullptr);

    gorc::pipe p;

    p.close_input();

    std::string out_msg = "Hello, World!";
    try {
        p.get_output().write_some(out_msg.data(), out_msg.size());
    }
    catch(std::system_error const &) {
        return;
    }
    catch(...) {
        assert_always("did not throw system error");
    }

    assert_always("did not throw exception");
}

test_case(eof_test)
{
    gorc::pipe p;

    p.close_output();

    int buf;
    size_t res = p.get_input().read_some(&buf, sizeof(buf));

    assert_eq(res, size_t(0));
    assert_true(p.get_input().at_end());
}

end_suite(pipe_test);
