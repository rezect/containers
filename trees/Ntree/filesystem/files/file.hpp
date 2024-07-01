#pragma once

#include <string>

namespace filesystem {

class File {
public:
    explicit File(std::string name) : name_(name){};

    void ReadAll() const;

    // overwrite
    void Write(std::string new_content = "");

    void Append(std::string new_content = "");

private:
    std::string name_;
    std::string content_;
};

}  // end namespace filesystem