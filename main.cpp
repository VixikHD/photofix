#include <iostream>
#include <libheif/heif.h>
//#include <libheif/heif_cxx.h>
#include <filesystem>
#include <string>
#include <png.h>
#include <cstring>
#include "HeifFile.h"

namespace fs = std::filesystem;

void convertHeic(unsigned int counter, const fs::path &path) {
	std::string outputPath = std::to_string(counter) + ".png";

	std::cout << "Converting '" << path << "' to '" << outputPath << "'..." << std::endl;
	HeifFile heif(path);
	heif.open();

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,nullptr, nullptr);
	if (!png_ptr) {
		fprintf(stderr, "libpng initialization failed (1)\n");
		return;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, nullptr);
		fprintf(stderr, "libpng initialization failed (2)\n");
		return;
	}

	FILE* fp = fopen(outputPath.c_str(), "wb");
	if (!fp) {
		fprintf(stderr, "Can't open %s: %s\n", outputPath.c_str(), std::strerror(errno));
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		fprintf(stderr, "Error while encoding image\n");
		return;
	}

	png_init_io(png_ptr, fp);

	bool withAlpha = (heif_image_get_chroma_format(heif.getImage()) == heif_chroma_interleaved_RGBA ||
					  heif_image_get_chroma_format(heif.getImage()) == heif_chroma_interleaved_RRGGBBAA_BE);

	int width = heif_image_get_width(heif.getImage(), heif_channel_interleaved);
	int height = heif_image_get_height(heif.getImage(), heif_channel_interleaved);

	int bitDepth;
	int input_bpp = heif_image_get_bits_per_pixel_range(heif.getImage(), heif_channel_interleaved);
	if (input_bpp > 8) {
		bitDepth = 16;
	}
	else {
		bitDepth = 8;
	}

	const int colorType = withAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;

	png_set_IHDR(png_ptr, info_ptr, width, height, bitDepth, colorType,
				 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// --- write ICC profile

	size_t profile_size = heif_image_handle_get_raw_color_profile_size(heif.getHandle());
	if (profile_size > 0) {
		uint8_t* profile_data = static_cast<uint8_t*>(malloc(profile_size));
		heif_image_handle_get_raw_color_profile(heif.getHandle(), profile_data);
		char profile_name[] = "unknown";
		png_set_iCCP(png_ptr, info_ptr, profile_name, PNG_COMPRESSION_TYPE_BASE,
					 (png_const_bytep) profile_data,
					 (png_uint_32) profile_size);
		free(profile_data);
	}


//	 --- write EXIF metadata
//
//#ifdef PNG_eXIf_SUPPORTED
//	size_t exifsize = 0;
//	uint8_t* exifdata = GetExifMetaData(handle, &exifsize);
//	if (exifdata) {
//		if (exifsize > 4) {
//			uint32_t skip = (exifdata[0]<<24) | (exifdata[1]<<16) | (exifdata[2]<<8) | exifdata[3];
//			skip += 4;
//
//			uint8_t* ptr = exifdata + skip;
//			size_t size = exifsize - skip;
//
//			// libheif by default normalizes the image orientation, so that we have to set the EXIF Orientation to "Horizontal (normal)"
//			modify_exif_orientation_tag_if_it_exists(ptr, (int)size, 1);
//
//			png_set_eXIf_1(png_ptr, info_ptr, (png_uint_32)size, ptr);
//		}
//
//		free(exifdata);
//	}
//#endif

	png_write_info(png_ptr, info_ptr);

	uint8_t** row_pointers = new uint8_t* [height];

	int stride_rgb;
	const uint8_t* row_rgb = heif_image_get_plane_readonly(heif.getImage(),
														   heif_channel_interleaved, &stride_rgb);

	for (int y = 0; y < height; ++y) {
		row_pointers[y] = const_cast<uint8_t*>(&row_rgb[y * stride_rgb]);
	}

	if (bitDepth == 16) {
		// shift image data to full 16bit range

		int shift = 16 - input_bpp;
		if (shift > 0) {
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < stride_rgb; x += 2) {
					uint8_t* p = (&row_pointers[y][x]);
					int v = (p[0] << 8) | p[1];
					v = (v << shift) | (v >> (16 - shift));
					p[0] = (uint8_t) (v >> 8);
					p[1] = (uint8_t) (v & 0xFF);
				}
			}
		}
	}


	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, nullptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	delete[] row_pointers;
	fclose(fp);
}

int main() {
	std::cout << "Scanning directory..." << std::endl;

	if(!fs::exists("./output"))
		fs::create_directory("./output");

	unsigned int counter = 0;

	std::string directoryPath = "./";
	try {
		for(const auto &entry : fs::directory_iterator(directoryPath)) {
			if(!fs::is_regular_file(entry)) {
				continue;
			}

			std::string ext = entry.path().filename().extension().string();
			if(ext == ".heic" || ext == ".HEIC") {
				convertHeic(++counter, entry.path());
				continue;
			}
		}
	} catch(const std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

    return 0;
}
