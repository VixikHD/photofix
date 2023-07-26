//
// Created by Vojtěch Stehlík on 23.07.2023.
//

#ifndef PHOTOFIX_HEIFFILEHOLDER_H
#define PHOTOFIX_HEIFFILEHOLDER_H


#include <string>
#include <filesystem>
#include <utility>
#include <libheif/heif.h>
#include "FileHolder.h"

class HeifFileHolder : public FileHolder {
    heif_context* ctx_ = nullptr;
    heif_image_handle* handle_ = nullptr;
    heif_image* image_ = nullptr;

    int width_ = 0, height_ = 0;

    int stride_ = 0;
    cv::Mat mat_;
public:
    explicit HeifFileHolder(std::filesystem::path path_): FileHolder(std::move(path_)) {}

    ~HeifFileHolder() {
        heif_image_release(image_);
        heif_image_handle_release(handle_);
        heif_context_free(ctx_);
    }

    void open() override {
        ctx_ = heif_context_alloc();

        heif_context_read_from_file(ctx_, path_.string().c_str(), nullptr);
        heif_context_get_primary_image_handle(ctx_, &handle_);
        heif_decode_image(handle_, &image_, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);

        width_ = heif_image_handle_get_width(handle_);
        height_ = heif_image_handle_get_height(handle_);

        auto* imageData = const_cast<uint8_t*>(heif_image_get_plane_readonly(image_, heif_channel_interleaved, &stride_));
        cv::Mat tmp(height_, width_, CV_8UC3, imageData,  (size_t)stride_);
        cv::cvtColor(tmp, mat_, cv::COLOR_RGB2BGR);
    }

    [[nodiscard]] const cv::Mat &asMatrix() const override {
        return mat_;
    }

    [[nodiscard]] int getWidth() const override {
        return width_;
    }

    [[nodiscard]] int getHeight() const override {
        return height_;
    }
};


#endif //PHOTOFIX_HEIFFILEHOLDER_H