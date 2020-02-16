#include "boost/filesystem/operations.hpp"
#include <boost/regex.hpp>
#include <unordered_map>

using namespace boost;

namespace fs = boost::filesystem;

int get_images_by_class(std::string **files, std::string directory_name, std::string class_name) {
    std::unordered_map<std::string, int> file_names = {};
    boost::regex pattern(class_name +"[0-9]+.BMP");

    for (const auto &entry : fs::directory_iterator(directory_name)) {
        std::string text = entry.path().c_str();
        boost::smatch what;
        if (regex_search(text, what, pattern)) {
            file_names.insert({what[0], 1});
        }
    }

    int num_classes = file_names.size();
    *files = new std::string[num_classes];

    int i = 0;
    for (const auto &n : file_names) {
        (*files)[i] = n.first;
        i++;
    }

    return num_classes;
}