#pragma once

#include "Contracts.h"
#include "Template.h"
#include <filesystem>
#include <fstream>
#include <optional>
#include <sol.hpp>
#include <string>
#include <vector>

namespace lua_template {

class Parser
{
public:
    auto loadTemplate(const std::filesystem::path& pathToTemplate) -> Template
    {
        std::ifstream input(pathToTemplate);
        if (input.good())
        {
            std::string content((std::istreambuf_iterator<char>(input)),
                                (std::istreambuf_iterator<char>()));
            return loadTemplateFromString(content);
        }
        else
        {
            reportCouldNotOpenTemplate(pathToTemplate.string());
        }
    }

    auto loadTemplateFromString(const std::string& templateString) -> Template
    {
        reset();
        m_string = &templateString;
        m_pos = m_string->cbegin();

        while (isStringLeft())
        {
            if (fetch() == '{')
            {
                if (!parseTemplateElement())
                {
                    return {};
                }
            }
            else
            {
                m_stringElement += fetchThenConsume();
            }
        }
        appendStringElement();
        constructLuaCode();
        return Template{m_luaCode, std::move(m_stringElements)};
    }

    auto luaCode() const -> const std::string& { return m_luaCode; }

    auto luaBlock(size_t index) const -> const std::string&
    {
        LT_ExpectsRange(index < luaBlockCount());
        return m_luaBlocks[index];
    }

    auto luaBlockCount() const -> size_t { return m_luaBlocks.size(); }

    auto staticString(size_t index) const -> const std::string&
    {
        LT_ExpectsRange(index < staticStringCount());
        return m_stringElements[index];
    }

    auto staticStringCount() const -> size_t { return m_stringElements.size(); }

private:
    Template m_template;

    void reset()
    {
        m_luaBlocks.clear();
        m_stringElements.clear();
        m_luaCode.clear();
        m_stringElement.clear();
    }

    void initializeLuaCode()
    {
        m_luaCode = "function generate(engine) local luatemplate = engine\n";
    }

    void constructLuaCode()
    {
        initializeLuaCode();
        for (auto const& block : m_luaBlocks)
        {
            m_luaCode += block + " ";
        }
        finalizeLuaCode();
    }

    void finalizeLuaCode() { m_luaCode += "\nend"; }

    bool parseTemplateElement()
    {
        LT_Expects(fetch(0) == '{');
        if (fetch(1) == '{')
        {
            if (fetch(2) == '=')
            {
                consume(3);
                appendStringElement();
                return parseLuaExpressionElement();
            }
            else
            {
                consume(2);
                appendStringElement();
                return parseLuaBlockElement();
            }
        }
        else
        {
            m_stringElement += fetchThenConsume();
            return true;
        }
    }

    bool parseLuaBlockElement()
    {
        if (auto maybeLuaCode = retrieveLuaCode())
        {
            m_luaBlocks.push_back(*maybeLuaCode);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool parseLuaExpressionElement()
    {
        if (auto maybeLuaCode = retrieveLuaCode())
        {
            auto const wrapperCode = "engine.append(" + *maybeLuaCode + ")\n";
            m_luaBlocks.push_back(wrapperCode);
            return true;
        }
        else
        {
            return false;
        }
    }

    auto retrieveLuaCode() -> std::optional<std::string>
    {
        std::string luaCode;
        while (isStringLeft())
        {
            if (fetch(0) == '}' && fetch(1) == '}')
            {
                consume(2);
                return luaCode;
            }
            luaCode += fetch();
            consume();
        }
        reportUnexpectedEndOfInput();
        return {};
    }

    void appendStringElement()
    {
        if (!m_stringElement.empty())
        {
            m_luaBlocks.push_back("engine.append_static_string(" +
                                  std::to_string(m_stringElements.size()) + ")\n");
            m_stringElements.push_back(std::move(m_stringElement));
            m_stringElement.clear();
        }
    }

    std::vector<std::string> m_stringElements;
    std::vector<std::string> m_luaBlocks;
    std::string m_stringElement;
    std::string m_luaCode;

    bool isStringLeft() const { return position() < stringLength(); }

    auto fetch(size_t index = 0) const -> std::string::value_type
    {
        if (position() + index < stringLength())
        {
            return *(m_pos + index);
        }
        else
        {
            return '\0';
        }
    }

    auto fetchThenConsume() -> std::string::value_type
    {
        auto c = fetch();
        consume();
        return c;
    }

    void consume(size_t count = 1) { m_pos += count; }

    auto position() const -> size_t { return m_pos - m_string->cbegin(); }

    auto stringLength() const -> size_t { return m_string->length(); }

    const std::string* m_string{nullptr};
    std::string::const_iterator m_pos;

    [[noreturn]] void reportCouldNotOpenTemplate(const std::string& filename) {
        throw std::runtime_error{"could not load template '" + filename + "'"};
    }

        [[noreturn]] void reportUnexpectedEndOfInput()
    {
        throw std::runtime_error{"unexpected end of input during parsing of template"};
    }
};

} // namespace lua_template