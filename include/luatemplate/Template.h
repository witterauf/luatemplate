#pragma once

#include <filesystem>
#include <optional>
#include <vector>

namespace lua_template {

class Template
{
public:
    Template() = default;
    explicit Template(const std::string& code, std::vector<std::string>&& staticStrings)
        : m_staticStrings{staticStrings}
        , m_luaCode{code}
    {
    }

    auto luaCode() const -> const std::string&
    {
        return m_luaCode;
    }

    auto staticString(size_t index) const -> const std::string&
    {
        return m_staticStrings[index];
    }

    auto staticStringCount() const -> size_t
    {
        return m_staticStrings.size();
    }

private:
    std::vector<std::string> m_staticStrings;
    std::string m_luaCode;
};

} // namespace lua_template