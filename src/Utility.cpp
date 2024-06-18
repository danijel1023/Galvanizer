#include "Utility.h"

namespace Galvanizer
{

namespace Utility
{

bool ValidateKeys(std::string_view str)
{
    for (size_t i = 0, last_i = 0; i < str.size(); i++)
    {
        if (str[i] != '.' && i + 1 != str.size())
            continue;

        // Test for "foo."
        if (str[i] == '.' && i + 1 == str.size())
            return false;

        // Test for "foo..bar", ".bar"
        if (i - last_i == 0)
            return false;

        last_i = i + 1;
    }

    return true;
}


// `str` is already correctly formatted (checked by `ValidateKeys`)
std::vector<std::string> ExtractKeys(std::string_view str)
{
    std::vector<std::string> keys;
    for (size_t i = 0, last_i = 0; i < str.size(); i++)
    {
        if (str[i] != '.' && i + 1 != str.size())
            continue;

        if (i + 1 != str.size())
            keys.emplace_back(str.substr(last_i, i - last_i));
        else
            keys.emplace_back(str.substr(last_i, i + 1 - last_i));

        last_i = i + 1;
    }

    return keys;
}

const char* strBool(bool x)
{
    if (x)
        return "true";
    else
        return "false";
}

}
}
