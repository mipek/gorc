#include "shader_loader.h"
#include "framework/content/assets/shader.h"
#include "framework/diagnostics/helper.h"
#include "framework/io/exception.h"
#include "framework/utility/make_unique.h"
#include <vector>
#include <cstdint>

const std::vector<boost::filesystem::path> gorc::content::loaders::shader_loader::asset_root_path = { "misc/glsl" };

std::unique_ptr<gorc::content::asset> gorc::content::loaders::shader_loader::deserialize(io::read_only_file& file, manager&, diagnostics::report& report) {
    std::vector<char> shader_text(file.get_size() + 1, '\0');
    file.read(shader_text.data(), file.get_size());

    const char* vshader[2] = { "#define VERTEXPROGRAM\n", shader_text.data() };
    const char* fshader[2] = { "#define FRAGMENTPROGRAM\n", shader_text.data() };

    GLint result = GL_FALSE;

    auto vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 2, vshader, nullptr);
    glCompileShader(vertexShaderObject);

    auto fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 2, fshader, nullptr);
    glCompileShader(fragmentShaderObject);

    bool failed = false;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        failed = true;

        GLint err_len;
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetShaderInfoLog(vertexShaderObject, err_len, nullptr, err_msg.data());

        report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
    }

    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        failed = true;

        GLint err_len;
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &err_len);
        std::vector<char> err_msg(err_len, '\0');
        glGetShaderInfoLog(fragmentShaderObject, err_len, nullptr, err_msg.data());

        report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
    }

    if(failed) {
        throw io::file_corrupt_exception();
    }

    auto programObject = glCreateProgram();
    glAttachShader(programObject, vertexShaderObject);
    glAttachShader(programObject, fragmentShaderObject);
    glLinkProgram(programObject);

    glGetProgramiv(programObject, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
            failed = true;

            GLint err_len;
            glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &err_len);
            std::vector<char> err_msg(err_len, '\0');
            glGetProgramInfoLog(programObject, err_len, nullptr, err_msg.data());

            report.add_error("ShaderLoader", err_msg.data(), diagnostics::error_location(file.Filename, 0, 0, 0, 0));
        }

    if(failed) {
        throw io::file_corrupt_exception();
    }

    glDeleteShader(vertexShaderObject);
    glDeleteShader(fragmentShaderObject);

    return make_unique<assets::shader>(programObject);
}
