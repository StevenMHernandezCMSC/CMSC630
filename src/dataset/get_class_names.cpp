#include "boost/filesystem/operations.hpp"
#include <boost/regex.hpp>
#include <unordered_map>

using namespace boost;

namespace fs = boost::filesystem;

int get_class_names(std::string **classes, std::string directory_name) {
    std::unordered_map<std::string, int> class_names = {};
    boost::regex pattern("([A-z]+)[0-9]+.BMP");

    for (const auto &entry : fs::directory_iterator(directory_name)) {
        std::string text = entry.path().c_str();
        boost::smatch what;
        if (regex_search(text, what, pattern)) {
            class_names.insert({what[1], 1});
        }
    }

    int num_classes = class_names.size();
    *classes = new std::string[num_classes];

    int i = 0;
    for (const auto &n : class_names) {
        (*classes)[i] = n.first;
        i++;
    }

    return num_classes;
}