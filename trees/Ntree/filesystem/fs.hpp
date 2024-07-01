#pragma once

#include <sstream>
#include <vector>

#include "detail/exceptions.hpp"
#include "files/directory.hpp"

namespace filesystem {

class Fs {
public:
    void ChangeDir(const std::string& path);

    Fs() {
        this->root_ = new Directory("", nullptr);
        this->current_ = root_;
    };

    void PWD() const;

    void RemoveFile(const std::string& path);

    void ListFiles(const std::string& path = ".") const;

    void MakeDir(const std::string& path, bool is_create_parents = false);

    void CreateFile(const std::string& path = ".", bool is_overwrite = false);

    void WriteToFile(const std::string& filename, bool is_overwrite = false,
                     std::ostringstream& stream = *(new std::ostringstream()));

    void ShowFileContent(const std::string& path);

    void FindFile(const std::string& filename);

    ~Fs() {
        for (std::pair<const std::string&, File*> i : root_->files_.Values()) {
            delete i.second;
        }
        root_->files_.Clear();
        for (std::pair<const std::string&, Directory*> i : root_->childs_.Values()) {
            this->RecursionDelete(i.second);
        }
        root_->childs_.Clear();
        delete root_;
    };

private:
    std::vector<std::string> Split(const std::string& str, const std::string& splitter = "/") const;

    Directory* ChangeDirForIter(Directory* iter, const std::string& path) const;

    Directory* FindParent(Directory* iter, const std::vector<std::string>& dirs) const;

    void RecursionFindFile(Directory* cur, const std::string& filename, bool& is_file_exist);

    void RecursionDelete(Directory* cur_dir);

    void WritePathToFile(Directory* parent, std::string filename);

    bool IsDirExist(Directory* iter, std::string name) const;

    bool IsFileExist(Directory* iter, std::string name_dir) const;

private:
    Directory* root_;
    Directory* current_;
};

}  // end namespace filesystem