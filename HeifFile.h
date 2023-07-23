//
// Created by Vojtěch Stehlík on 23.07.2023.
//

#ifndef PHOTOFIX_HEIFFILE_H
#define PHOTOFIX_HEIFFILE_H


#include <string>
#include <filesystem>
#include <utility>
#include <libheif/heif.h>

class HeifFile {
	std::filesystem::path path_;

	heif_context* ctx_ = nullptr;
	heif_image_handle* handle_ = nullptr;
	heif_image* image_ = nullptr;

	size_t width_ = 0, height_ = 0;
public:
	explicit HeifFile(std::filesystem::path  path_): path_(std::move(path_)) {}
	~HeifFile() {
		heif_image_release(image_);
		heif_image_handle_release(handle_);
		heif_context_free(ctx_);
	}

	void open() {
		ctx_ = heif_context_alloc();

		heif_context_read_from_file(ctx_, path_.string().c_str(), nullptr);
		heif_context_get_primary_image_handle(ctx_, &handle_);

		width_ = heif_image_handle_get_width(handle_);
		height_ = heif_image_handle_get_height(handle_);
	}

	const struct heif_context* getContext() {
		return ctx_;
	}

	const struct heif_image_handle* getHandle() {
		return handle_;
	}

	const struct heif_image* getImage() {
		return image_;
	}

	[[nodiscard]] size_t getWidth() const {
		return width_;
	}

	[[nodiscard]] size_t getHeight() const {
		return height_;
	}
};


#endif //PHOTOFIX_HEIFFILE_H
