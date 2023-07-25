//
// Created by vix on 25.7.23.
//

#ifndef PHOTOFIX_OPENCVFILEHOLDER_H
#define PHOTOFIX_OPENCVFILEHOLDER_H


#include <opencv2/opencv.hpp>
#include "FileHolder.h"

class OpenCVFileHolder : public FileHolder {
    cv::Mat mat_;
public:
    explicit OpenCVFileHolder(std::filesystem::path path_): FileHolder(std::move(path_)) {}

    void open() {
        mat_ = cv::imread(path_);
    }

    [[nodiscard]] const cv::Mat &asMatrix() const override {
        return mat_;
    }

    [[nodiscard]] int getWidth() const override {
        return mat_.size().width;
    }

    [[nodiscard]] int getHeight() const override {
        return mat_.size().height;
    }
};


#endif //PHOTOFIX_OPENCVFILEHOLDER_H
