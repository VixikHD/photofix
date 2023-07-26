//
// Created by vix on 24.7.23.
//

#ifndef PHOTOFIX_FORMATCONVERTER_H
#define PHOTOFIX_FORMATCONVERTER_H


#include <string>
#include <utility>
#include <filesystem>
#include <vector>
#include <future>
#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "format/HeifFileHolder.h"
#include "format/OpenCVFileHolder.h"

class FormatConverter {
    std::string inputPath_;
    std::string outputPath_;

    std::mutex mtx_;

    void lazyInitOutputPath() {
        if(!std::filesystem::exists(outputPath_)) {
            std::filesystem::create_directory(outputPath_);
        }
    }

    bool processFile(uint counter, const std::filesystem::path &path) {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

        std::string ext = path.extension().string();
        std::string outputPath = outputPath_ + "/" + std::to_string(counter) + ".webp";

        std::unique_ptr<FileHolder> fileHolder;
        if(ext == ".heic" || ext == ".HEIC") {
            fileHolder = std::make_unique<HeifFileHolder>(path);
        } else if(ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".JPG" || ext == ".JPEG" || ext == ".PNG") {
            fileHolder = std::make_unique<OpenCVFileHolder>(path);
        } else {
            return false;
        }

        fileHolder->open();
        saveFile(outputPath, *fileHolder);

        {
            std::lock_guard<std::mutex> lock(mtx_);

            std::cout << "Converted \"" << path.string() << "\" to \"" << outputPath << "\" in " << (
                (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0
            ) << " sec." << std::endl;
        }
        return true;
    }

    void calculateResizedDimensions(int &width, int &height) {
        const int BASE_MAX_HIGHER_SIDE = 1920;
        if(width > height) {
            if(width < BASE_MAX_HIGHER_SIDE) {
                return;
            }

            float scaleX = (float)BASE_MAX_HIGHER_SIDE / (float)width;
            width = BASE_MAX_HIGHER_SIDE;
            height = (int)((float)height * scaleX);
        } else {
            if(height < BASE_MAX_HIGHER_SIDE) {
                return;
            }

            float scaleY = (float)BASE_MAX_HIGHER_SIDE / (float)height;
            height = BASE_MAX_HIGHER_SIDE;
            width = (int)((float)width * scaleY);
        }
    }

    void saveFile(const std::string &outputPath, const FileHolder &holder) {
        int width = holder.getWidth();
        int height = holder.getHeight();

        calculateResizedDimensions(width, height);

        cv::Mat output;
        cv::resize(holder.asMatrix(), output, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);

        std::vector<int> params;
        params.push_back(cv::IMWRITE_WEBP_QUALITY);
        params.push_back(95); // Quality

        cv::imwrite(outputPath, output, params);
    }

public:
    FormatConverter(std::string inputPath, std::string outputPath)
        : inputPath_(std::move(inputPath)), outputPath_(std::move(outputPath)) {}

    void process(int maximumProcessCount = 8) {
        lazyInitOutputPath();

        std::chrono::time_point start = std::chrono::steady_clock::now();

        uint counter = 0;

        std::vector<std::future<void>> futures;
        std::queue<std::filesystem::path> sourceQueue;
        for(const auto &entry : std::filesystem::directory_iterator(inputPath_)) {
            if(!std::filesystem::is_regular_file(entry.path())) {
                continue;
            }

            sourceQueue.emplace(entry.path());
        }

        uint processingCounter = 0;
        while(!sourceQueue.empty()) {
            auto entry = sourceQueue.front();
            sourceQueue.pop();

            futures.emplace_back(std::async(std::launch::async, [this](uint counter, const std::filesystem::path &path){
                processFile(counter, path);
            }, ++counter, entry));
            ++processingCounter;

            while(processingCounter >= maximumProcessCount) {
                for(ssize_t i = (ssize_t)futures.size() - 1; i >= 0; --i) {
                    auto &future = futures[i];
                    if(future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                        --processingCounter;
                        future.get();
                        {
                            std::lock_guard<std::mutex> lock(mtx_);
                            std::cout << "Another image has been processed. There is currently " << processingCounter << " being processed.\n";
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        std::cout << "Processed " << counter << " files in " << (
            (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0
        ) << " sec." << std::endl;
    }
};


#endif //PHOTOFIX_FORMATCONVERTER_H
