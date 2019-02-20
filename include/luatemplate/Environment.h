#pragma once

#include "Parser.h"
#include "Template.h"
#include <filesystem>
#include <sol.hpp>
#include <vector>

namespace lua_template {

class Environment
{
public:
    explicit Environment(sol::this_state lua)
        : m_lua{lua}
    {
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

    auto include(const std::filesystem::path& fileName) const -> Template
    {
        namespace fs = std::filesystem;
        Parser parser;
        if (fileName.is_absolute())
        {
            if (fs::exists(fileName))
            {
                return parser.loadTemplate(fileName);
            }
        }
        else
        {
            for (auto const& includePath : m_includePaths)
            {
                auto const fullPath = includePath / fileName;
                if (fs::exists(fullPath))
                {
                    return parser.loadTemplate(fullPath);
                }
            }
        }
        throw std::runtime_error{"could not open template file '" + fileName.string() + "'"};
    }

private:
    sol::this_state m_lua;
    std::vector<std::filesystem::path> m_includePaths;
};

} // namespace lua_template