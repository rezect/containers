#pragma once

#include <vector>

#include "../map/map.hpp"
#include "file.hpp"

namespace filesystem {

class Directory {
public:
    friend class Fs;

    Directory(std::string name, Directory* parent)
        : name_(name), parent_(parent), childs_(Map<std::string, Directory*>()), files_(Map<std::string, File*>()) {
    }

    const std::string& GetName() const {
        return name_;
    }

    ~Directory() {
    }

private:
    std::string name_;
    Directory* parent_;
    Map<std::string, Directory*> childs_;
    Map<std::string, File*> files_;
};

}  // end namespace filesystem