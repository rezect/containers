#include "file.hpp"

namespace filesystem {

void File::ReadAll() const {
    printf("%s", content_.c_str());
}

void File::Write(std::string new_content) {
    content_ = new_content;
}

void File::Append(std::string new_content) {
    content_ += new_content;
}

}  // end namespace filesystem