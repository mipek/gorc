#include "pipe.hpp"
#include "log/log.hpp"
#include <system_error>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

gorc::pipe_input_stream::pipe_input_stream(int fd)
    : fd(fd)
{
    return;
}

gorc::pipe_input_stream::~pipe_input_stream()
{
    if(::close(fd) != 0) {
        // LCOV_EXCL_START
        // close should not be retried, even on EINTR
        LOG_ERROR(format("pipe_input_stream: %s") % std::generic_category().message(errno));
        // LCOV_EXCL_STOP
    }
}

size_t gorc::pipe_input_stream::read_some(void *dest, size_t size)
{
    // LCOV_EXCL_START
    ssize_t rv = 0;
    do {
    // LCOV_EXCL_STOP
        rv = ::read(fd, dest, size);
    // LCOV_EXCL_START
    }
    while(rv < 0 && errno == EINTR);
    // LCOV_EXCL_STOP

    if(rv == 0) {
        is_at_end = true;
    }

    if(rv >= 0) {
        return static_cast<size_t>(rv);
    }
    else {
        // LCOV_EXCL_START
        // system call
        throw std::system_error(errno, std::generic_category());
        // LCOV_EXCL_STOP
    }
}

bool gorc::pipe_input_stream::at_end()
{
    return is_at_end;
}

gorc::pipe_output_stream::pipe_output_stream(int fd)
    : fd(fd)
{
    return;
}

gorc::pipe_output_stream::~pipe_output_stream()
{
    if(::close(fd) != 0) {
        // close should not be retried, even on EINTR
        // LCOV_EXCL_START
        LOG_ERROR(format("pipe_output_stream: %s") % std::generic_category().message(errno));
        // LCOV_EXCL_STOP
    }
}

size_t gorc::pipe_output_stream::write_some(void const *src, size_t size)
{
    ssize_t rv = 0;
    do { // LCOV_EXCL_LINE
        rv = ::write(fd, src, size);
    }
    while(rv < 0 && errno == EINTR);

    if(rv >= 0) {
        return static_cast<size_t>(rv);
    }
    else {
        throw std::system_error(errno, std::generic_category());
    }
}

gorc::pipe::pipe()
{
    int pipefd[2];
    int result = ::pipe2(pipefd, O_CLOEXEC);

    // LCOV_EXCL_START - system call
    if(result != 0) {
        throw std::system_error(errno, std::generic_category());
    }
    // LCOV_EXCL_STOP

    input = std::make_unique<pipe_input_stream>(pipefd[0]);
    output = std::make_unique<pipe_output_stream>(pipefd[1]);
}

gorc::pipe::pipe(std::unique_ptr<pipe_input_stream> &&input,
                 std::unique_ptr<pipe_output_stream> &&output)
    : input(std::forward<std::unique_ptr<pipe_input_stream>>(input))
    , output(std::forward<std::unique_ptr<pipe_output_stream>>(output))
{
    return;
}

gorc::pipe_input_stream& gorc::pipe::get_input()
{
    return *input;
}

gorc::pipe_output_stream& gorc::pipe::get_output()
{
    return *output;
}

void gorc::pipe::close_input()
{
    if(!reusable) {
        input.reset();
    }
}

void gorc::pipe::close_output()
{
    if(!reusable) {
        output.reset();
    }
}

void gorc::pipe::set_reusable(bool is_reusable)
{
    reusable = is_reusable;
}

gorc::pipe gorc::make_input_file_pipe(path const &p)
{
    std::string native_filename = p.native();

    // LCOV_EXCL_START
    int res = -1;
    do {
    // LCOV_EXCL_STOP
        res = ::open(native_filename.c_str(), O_RDONLY | O_CLOEXEC);
    // LCOV_EXCL_START
    } while(res == -1 && errno == EINTR);
    // LCOV_EXCL_STOP

    if(res == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    return pipe(std::make_unique<pipe_input_stream>(res),
                nullptr);
}

gorc::pipe gorc::make_output_file_pipe(path const &p)
{
    std::string native_filename = p.native();

    // LCOV_EXCL_START
    int res = -1;
    do {
    // LCOV_EXCL_STOP
        res = ::open(native_filename.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT | O_TRUNC | O_CLOEXEC,
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    // LCOV_EXCL_START
    } while(res == -1 && errno == EINTR);
    // LCOV_EXCL_STOP

    if(res == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    return pipe(nullptr,
                std::make_unique<pipe_output_stream>(res));
}

gorc::pipe gorc::make_output_file_append_pipe(path const &p)
{
    std::string native_filename = p.native();

    // LCOV_EXCL_START
    int res = -1;
    do {
    // LCOV_EXCL_STOP
        res = ::open(native_filename.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT | O_CLOEXEC,
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    // LCOV_EXCL_START
    } while(res == -1 && errno == EINTR);
    // LCOV_EXCL_STOP

    if(res == -1) {
        throw std::system_error(errno, std::generic_category());
    }

    return pipe(nullptr,
                std::make_unique<pipe_output_stream>(res));
}
