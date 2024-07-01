#include "fs.hpp"

#include <iostream>
#include <sstream>

#include "files/file.cpp"

namespace filesystem {

void Fs::ChangeDir(const std::string& path) {
    current_ = this->ChangeDirForIter(current_, path);
}

void Fs::ListFiles(const std::string& path) const {
    Directory* iter = current_;
    if (path != ".") {
        iter = this->ChangeDirForIter(iter, path);
    }
    for (std::pair<const std::string, File*>& file : iter->files_.Values()) {
        std::cout << file.first << std::endl;
    }
}

void Fs::MakeDir(const std::string& path, bool is_create_parents) {
    Directory* iter = current_;
    std::vector<std::string> dirs = this->Split(path);
    for (std::string& next_dir_name : dirs) {
        if (next_dir_name == "/") {
            iter = root_;
        } else if (next_dir_name == "..") {
            if (iter == root_) {
                throw exceptions::FileNotFoundException("File isnt exist");
            }
            iter = iter->parent_;
        } else {
            if (this->IsDirExist(iter, next_dir_name)) {
                iter = (*iter->childs_.Find(next_dir_name)).second;
            } else {
                if (next_dir_name == dirs.back()) {
                    Directory* new_dir = new Directory(next_dir_name, iter);
                    iter->childs_.Insert(std::make_pair(next_dir_name, new_dir));
                } else if (is_create_parents) {
                    Directory* new_dir = new Directory(next_dir_name, iter);
                    iter->childs_.Insert(std::make_pair(next_dir_name, new_dir));
                    iter = new_dir;
                } else {
                    throw exceptions::FileNotFoundException("File isnt exist");
                }
            }
        }
    }
}

auto Fs::Split(const std::string& str, const std::string& splitter) const -> std::vector<std::string> {
    std::vector<std::string> result;
    size_t start_index = 0;
    size_t splitter_sz = splitter.size();
    size_t str_sz = str.size();
    if (str.substr(0, splitter_sz) == splitter) {
        result.push_back("/");
        start_index = splitter_sz;
    }
    for (size_t i = start_index; i < str_sz; ++i) {
        if (str.substr(i, splitter_sz) == splitter) {
            if (str.substr(i, splitter_sz).empty()) {
                throw exceptions::FileNotFoundException("File isnt exist");
            } else {
                result.push_back(str.substr(start_index, i - start_index));
                start_index = i + splitter_sz;
                i = start_index;
            }
        }
        if (i == str_sz - 1) {
            result.push_back(str.substr(start_index, i - start_index + 1));
        }
    }
    return result;
}

void Fs::RemoveFile(const std::string& path) {
    std::vector<std::string> dirs = this->Split(path);
    Directory* iter = this->FindParent(current_, dirs);
    if (iter == nullptr) {
        for (std::pair<const std::string&, File*> i : root_->files_.Values()) {
            delete i.second;
        }
        root_->files_.Clear();
        for (std::pair<const std::string&, Directory*> i : root_->childs_.Values()) {
            this->RecursionDelete(i.second);
        }
        root_->childs_.Clear();
    } else if (this->IsFileExist(iter, dirs.back())) {
        delete (*iter->files_.Find(dirs.back())).second;
        iter->files_.Erase(dirs.back());
    } else if (this->IsDirExist(iter, dirs.back())) {
        this->RecursionDelete((*iter->childs_.Find(dirs.back())).second);
        iter->childs_.Erase(dirs.back());
    } else {
        throw exceptions::FileNotFoundException("File isnt exist");
    }
}

void Fs::CreateFile(const std::string& path, bool is_overwrite) {
    std::vector<std::string> dirs = this->Split(path);
    Directory* iter = this->FindParent(current_, dirs);
    if (this->IsFileExist(iter, dirs.back())) {
        if (is_overwrite) {
            (*iter->files_.Find(dirs.back())).second->Write();
        } else {
            throw exceptions::FileNotFoundException("File isnt exist");
        }
    } else {
        File* new_file = new File(dirs.back());
        iter->files_.Insert(std::make_pair(dirs.back(), new_file));
    }
}

void Fs::WriteToFile(const std::string& path, bool is_overwrite, std::ostringstream& stream) {
    std::vector<std::string> dirs = this->Split(path);
    Directory* iter = this->FindParent(current_, dirs);
    if (this->IsFileExist(iter, dirs.back())) {
        if (is_overwrite) {
            (*iter->files_.Find(dirs.back())).second->Write(stream.str());
        } else {
            (*iter->files_.Find(dirs.back())).second->Append(stream.str());
        }
    } else {
        throw exceptions::FileNotFoundException("File isnt exist");
    }
}

void Fs::ShowFileContent(const std::string& path) {
    std::vector<std::string> dirs = this->Split(path);
    Directory* iter = this->FindParent(current_, dirs);
    if (this->IsFileExist(iter, dirs.back())) {
        (*iter->files_.Find(dirs.back())).second->ReadAll();
    } else {
        throw exceptions::FileNotFoundException("File isnt exist");
    }
}

void Fs::FindFile(const std::string& filename) {
    bool is_file_exist = false;
    RecursionFindFile(root_, filename, is_file_exist);
    if (!is_file_exist) {
        throw exceptions::FileNotFoundException("File isnt exist");
    }
}

void Fs::PWD() const {
    if (current_ == root_) {
        std::cout << "/" << std::endl;
        return;
    }
    Directory* iter = current_;
    std::string res = current_->GetName();
    while (iter->parent_ != nullptr) {
        iter = iter->parent_;
        res = iter->GetName() + '/' + res;
    }
    std::cout << res << std::endl;
}

Directory* Fs::ChangeDirForIter(Directory* iter, const std::string& path) const {
    for (std::string& next_dir_name : this->Split(path)) {
        if (next_dir_name == "/") {
            iter = root_;
        } else if (next_dir_name == "..") {
            if (iter == root_) {
                throw exceptions::FileNotFoundException("File isnt exist");
            }
            iter = iter->parent_;
        } else if (this->IsDirExist(iter, next_dir_name)) {
            iter = (*iter->childs_.Find(next_dir_name)).second;
        } else {
            throw exceptions::FileNotFoundException("File isnt exist");
        }
    }
    return iter;
}

Directory* Fs::FindParent(Directory* iter, const std::vector<std::string>& dirs) const {
    for (const std::string& next_dir_name : dirs) {
        if (next_dir_name == "/") {
            iter = root_;
        } else if (next_dir_name == "..") {
            if (iter == root_) {
                throw exceptions::FileNotFoundException("File isnt exist");
            }
            iter = iter->parent_;
        } else {
            if (next_dir_name == dirs.back()) {
                return iter;
            } else if (this->IsDirExist(iter, next_dir_name)) {
                iter = (*iter->childs_.Find(next_dir_name)).second;
            } else {
                throw exceptions::FileNotFoundException("File isnt exist");
            }
        }
    }
    return nullptr;
}

void Fs::RecursionDelete(Directory* cur_dir) {
    for (std::pair<const std::string&, File*> i : cur_dir->files_.Values()) {
        delete i.second;
    }
    for (std::pair<const std::string&, Directory*> i : cur_dir->childs_.Values()) {
        this->RecursionDelete(i.second);
    }
    delete cur_dir;
}

void Fs::RecursionFindFile(Directory* cur, const std::string& filename, bool& is_file_exist) {
    if (cur->GetName() == filename) {
        if (cur->parent_) {
            this->WritePathToFile(cur->parent_, filename);
            is_file_exist = true;
        } else {
            std::cout << "/" << std::endl;
        }
    }
    for (std::pair<const std::string, File*>& file : cur->files_.Values()) {
        if (file.first == filename) {
            this->WritePathToFile(cur, filename);
            is_file_exist = true;
        }
    }
    for (std::pair<const std::string, Directory*>& dir : cur->childs_.Values()) {
        this->RecursionFindFile(dir.second, filename, is_file_exist);
    }
}

void Fs::WritePathToFile(Directory* iter, std::string filename) {
    std::string res = filename;
    while (iter != nullptr) {
        res = iter->GetName() + '/' + res;
        iter = iter->parent_;
    }
    std::cout << res << std::endl;
}

bool Fs::IsDirExist(Directory* cur, std::string dir_name) const {
    return cur->childs_.Find(dir_name) != cur->childs_.End();
}

bool Fs::IsFileExist(Directory* cur, std::string file_name) const {
    return cur->files_.Find(file_name) != cur->files_.End();
}

}  // end namespace filesystem