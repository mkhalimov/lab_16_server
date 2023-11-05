#include <string>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

std::map<std::string, std::string> ROUTES = {
    {"/", "itworks.html"},
    {"/404", "error.html"},
};
