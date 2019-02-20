#pragma once

#include <stdexcept>

#define LT_STRINGIFY_HELPER(x) #x
#define LT_TOSTRING(x) LT_STRINGIFY_HELPER(x)

#define LT_Expects(cond)                                                                           \
    if (!(cond))                                                                                   \
        throw std::runtime_error{"Contract violation: precondition failure at " __FILE__           \
                                 ": " LT_TOSTRING(__LINE__)};
#define LT_ExpectsRange(cond)                                                                      \
    if (!(cond))                                                                                   \
        throw std::out_of_range{"Contract violation: range precondition failure at " __FILE__      \
                                ": " LT_TOSTRING(__LINE__)};
#define LT_Ensures(cond)                                                                           \
    if (!(cond))                                                                                   \
        throw std::runtime_error{"Contract violation: postcondition failure at " __FILE__          \
                                 ": " LT_TOSTRING(__LINE__)};
#define LT_InvalidCase(value)                                                                      \
    throw std::runtime_error{"Invalid value (" +                                                   \
                             std::to_string(static_cast<unsigned int>(value)) +                    \
                             ") in switch statement at " __FILE__ ": " LT_TOSTRING(__LINE__)};
