#pragma once

#include "Environment.h"
#include "Parser.h"
#include "Renderer.h"
#include "Template.h"
#include <filesystem>
#include <sol.hpp>

namespace lua_template {

inline auto render(const Template& tmpl, sol::this_state s) -> std::string
{
    Environment env{s};
    Renderer renderer;
    return renderer.render(tmpl, env);
}

inline void renderToFile(const Template& tmpl, sol::this_state s, const std::filesystem::path& path)
{
    Environment env{s};
    Renderer renderer;
    return renderer.renderToFile(tmpl, env, path);
}

} // namespace lua_template