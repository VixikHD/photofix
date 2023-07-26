//
// Created by vix on 24.7.23.
//

#ifndef PHOTOFIX_FILEHOLDER_H
#define PHOTOFIX_FILEHOLDER_H


#include <filesystem>
#include <utility>

class FileHolder {
protected:
    const std::filesystem::path path_;
public:
    explicit FileHolder(std::filesystem::path path_): path_(std::move(path_)) {}

    virtual void open() = 0;

    [[nodiscard]] virtual const cv::Mat &asMatrix() const  = 0;

    [[nodiscard]] virtual int getWidth() const = 0;

    [[nodiscard]] virtual int getHeight() const = 0;

    const std::filesystem::path& getPath() {
        return path_;
    }
};


#endif //PHOTOFIX_FILEHOLDER_H
