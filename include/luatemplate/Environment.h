#pragma once

#include "Parser.h"
#include "Template.h"
#include <filesystem>
#include <sol.hpp>
#include <vector>

namespace lua_template {

class Environment;
class Renderer;

namespace details {
void includeInto(Renderer* renderer, const Template& t, Environment& env);
}

class Environment
{
public:
    explicit Environment(sol::this_state lua)
        : m_lua{lua}
    {
        sol::state_view s{lua};
        auto luatemplate = s.create_table();
        luatemplate.set_function(
            "execute", [this](const std::string& filename) { return this->execute(filename); });
        luatemplate.set_function("include_template", [this](const std::string& filename) {
            this->includeTemplate(filename);
        });
        luatemplate.set_function("script_path", [this]() { return this->scriptPath(); });
        s["luatemplate"] = luatemplate;
    }

    void appendIncludePath(const std::filesystem::path& includePath)
    {
        m_includePaths.push_back(includePath);
    }

    template <class InputIterator> void appendIncludePaths(InputIterator begin, InputIterator end)
    {
        for (; begin != end; ++begin)
        {
            m_includePaths.push_back(*begin);
        }
    }

    auto lua() const -> sol::this_state { return m_lua; }

    auto findFile(const std::filesystem::path& filename) const
        -> std::optional<std::filesystem::path>
    {
        namespace fs = std::filesystem;
        if (filename.is_absolute())
        {
            if (fs::exists(filename))
            {
                return filename;
            }
        }
        else
        {
            return findFileInIncludePaths(filename);
        }
        return {};
    }

    auto findRelativeFile(const std::filesystem::path& current,
                          const std::filesystem::path& pathToFind) const
        -> std::optional<std::filesystem::path>
    {
        namespace fs = std::filesystem;
        if (pathToFind.is_absolute())
        {
            if (fs::exists(pathToFind))
            {
                return pathToFind;
            }
        }
        else
        {
            std::filesystem::path fullPath;
            if (current.has_filename())
            {
                fullPath = current.parent_path() / pathToFind;
            }
            else
            {
                fullPath = current / pathToFind;
            }
            if (fs::exists(fullPath))
            {
                return fullPath;
            }
            return findFileInIncludePaths(pathToFind);
        }
        return {};
    }

    auto include(const std::filesystem::path& filename) const -> Template
    {
        if (auto const maybeFile = findFile(filename))
        {
            Parser parser;
            return parser.loadTemplate(*maybeFile);
        }
        else
        {
            throw std::runtime_error{"Could not find template file '" + filename.string() + "'"};
        }
    }

    auto execute(const std::filesystem::path& filename) -> sol::unsafe_function_result
    {
        if (auto const maybeFile = findFile(filename))
        {
            sol::state_view lua{m_lua};
            m_includeStack.push_back(*maybeFile);
            auto result = lua.script_file(maybeFile->string());
            m_includeStack.pop_back();
            return result;
        }
        else
        {
            throw std::runtime_error{"Could not find script file '" + filename.string() + "'"};
        }
    }

    auto stackLevel() const -> size_t { return m_includeStack.size(); }
    void push(const std::filesystem::path& file) { m_includeStack.push_back(file); }
    void pop() { m_includeStack.pop_back(); }
    void pushRenderer(Renderer* renderer) { m_renderers.push_back(renderer); }
    void popRenderer() { m_renderers.pop_back(); }
    auto currentScript() const -> std::filesystem::path
    {
        LT_Expects(!m_includeStack.empty());
        return m_includeStack.back();
    }

    auto scriptPath() const -> std::string { return currentScript().parent_path().string(); }

private:
    auto findFileInIncludePaths(const std::filesystem::path& filename) const
        -> std::optional<std::filesystem::path>
    {
        LT_Expects(filename.is_relative());
        for (auto const& includePath : m_includePaths)
        {
            auto const fullPath = includePath / filename;
            if (std::filesystem::exists(fullPath))
            {
                return fullPath;
            }
        }
        return {};
    }

    void includeTemplate(const std::filesystem::path& filename)
    {
        auto const templ = include(filename);
        details::includeInto(m_renderers.back(), templ, *this);
    }

    sol::this_state m_lua;
    std::vector<std::filesystem::path> m_includePaths;
    std::vector<std::filesystem::path> m_includeStack;
    std::vector<Renderer*> m_renderers;
};

} // namespace lua_template