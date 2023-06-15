#include <filesystem>
#include <iostream>
#include <string>

#include "image.h"
#include "bitmap.h"
#include "gif.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "imed - CLI image editor. Support BMP and GIF87a.\n Usage: imed <input file> <command> [<args>]\nAvailable commands: crop, flip, rcw, rccw, grayscale, inverse, resize." << std::endl;
        return 1;
    }

    std::string command = argv[2];

    std::filesystem::path input_file = argv[1];

    bool is_gif = false;

    if (input_file.extension() == ".gif") {
        is_gif = true;
    } else if (input_file.extension() != ".bmp") {
        std::cout << "Given file is not a GIF or Bitmap" << std::endl;
        return 1;
    }

    if (!is_gif) {
        Bitmap bitmap(input_file.string());
        if (command == "crop") {
            if (argc < 7) {
                std::cout << "Incorrect arguments to crop!\nUsage: imed <input file> crop <x0> <y0> <width> <height>";
                return 1;
            }

            try {
                int x0 = std::stoi(argv[3]);
                int y0 = std::stoi(argv[4]);
                int width = std::stoi(argv[5]);
                int height = std::stoi(argv[6]);

                bitmap.crop(x0, y0, width, height).write(input_file.string());
            } catch (...) {
                std::cout << "Incorrect arguments to crop!\nUsage: imed <input file> crop <x0> <y0> <width> <height>";
                return 1;
            }
        } else if (command == "flip") {
            bitmap.flip().write(input_file.string());
        } else if (command == "rcw") {
            bitmap.rotate_clockwise().write(input_file.string());
        } else if (command == "rccw") {
            bitmap.rotate_counterclockwise().write(input_file.string());
        } else if (command == "grayscale") {
            bitmap.grayscale().write(input_file.string());
        } else if (command == "inverse") {
            bitmap.inverse().write(input_file.string());
        } else if (command == "resize") {
            if (argc < 6) {
                std::cout << "Incorrect arguments to resize!\nUsage: imed <input file> resize <width> <height> <nn|bil|bic>";
                return 1;
            }

            try {
                int width = std::stoi(argv[3]);
                int height = std::stoi(argv[4]);
                std::string mode_str = argv[5];
                if (mode_str == "nn") {
                    bitmap.resize(width, height, Interpolation::NearestNeighbour).write(input_file.string());
                } else if (mode_str == "bil") {
                    bitmap.resize(width, height, Interpolation::Bilinear).write(input_file.string());
                } else if (mode_str == "bic") {
                    bitmap.resize(width, height, Interpolation::Bicubic).write(input_file.string());
                } else {
                    throw;
                }

            } catch (...) {
                std::cout << "Incorrect arguments to resize!\nUsage: imed <input file> resize <width> <height> <nn|bil|bic>";
                return 1;
            }
        } else if (command == "test") {
            std::cout << "Testing write\n" << std::endl;
            bitmap.write(input_file.string());
        }
    }
    /* if (is_gif) {
        GIF gif{input_file.string()};
        if (command == "crop") {
            gif.crop(0, 0, 100, 100).write(input_file.string());
        } else if (command == "flip") {
            gif.flip().write(input_file.string());
        } else if (command == "rcw") {
            gif.rotate_clockwise().write(input_file.string());
        } else if (command == "rccw") {
            gif.rotate_counterclockwise().write(input_file.string());
        } else if (command == "grayscale") {
            gif.grayscale().write(input_file.string());
        } else if (command == "inverse") {
            gif.inverse().write(input_file.string());
        } else if (command == "resize") {
            gif.resize(560, 560, Interpolation::NearestNeighbour).write(input_file.string());
        }
    }*/

    return 0;
}
