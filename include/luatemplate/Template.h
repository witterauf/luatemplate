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

    bool hasSourceFile() const { return m_source.has_value(); }
    void setSourceFile(const std::filesystem::path& filename) { m_source = filename; }
    auto sourceFile() const { return *m_source; }

    auto luaCode() const -> const std::string& { return m_luaCode; }
    auto staticString(size_t index) const -> const std::string& { return m_staticStrings[index]; }
    auto staticStringCount() const -> size_t { return m_staticStrings.size(); }

private:
    std::optional<std::filesystem::path> m_source;
    std::vector<std::string> m_staticStrings;
    std::string m_luaCode;
};

} // namespace lua_template