#pragma once

#include "Contracts.h"
#include "Environment.h"
#include "Parser.h"
#include "Template.h"
#include <fstream>
#include <string>

namespace lua_template {

class Renderer
{
public:
    void renderToFile(const Template& t, Environment& env, const std::filesystem::path& path)
    {
        auto const rendered = render(t, env);
        std::ofstream output{path};
        if (output.good())
        {
            output << rendered;
        }
        else
        {
            throw std::runtime_error{"could not open '" + path.string() + "' for writing"};
        }
    }

    auto render(const Template& t, Environment& env) -> std::string
    {
        LT_Expects(!t.luaCode().empty());
        m_template = &t;
        m_environment = &env;

        sol::state_view lua{env.lua()};
        lua.script(t.luaCode());
        sol::table engine = lua.create_table();
        engine.set_function("append",
                            sol::overload([this](const std::string& value) { this->append(value); },
                                          [this](long long value) { this->append(value); }));
        engine.set_function("append_static_string",
                            [this](size_t index) { this->appendStatic(index); });
        engine.set_function("include_template", [this](const std::string& fileName) {
            this->includeTemplate(fileName);
        });
        m_output.clear();
        lua["generate"](engine);
        lua["generate"] = sol::nil;
        return m_output;
    }

private:
    void append(const std::string& value) { m_output += value; }
    void append(long long value) { m_output += std::to_string(value); }
    void appendStatic(size_t index) { m_output += m_template->staticString(index); }

    void includeTemplate(const std::string& fileName)
    {
        LT_Expects(m_environment);
        const std::filesystem::path& path{fileName};
        Renderer renderer;
        m_output += renderer.render(m_environment->include(path), *m_environment);
    }

    const Template* m_template{nullptr};
    Environment* m_environment{nullptr};
    std::string m_output;
};

} // namespace lua_template