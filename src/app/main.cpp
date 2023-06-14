#include <iostream>
#include <string>
#include <filesystem>

#include "image.h"
#include "bitmap.h"
#include "gif.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Help text" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    bool no_input = argc < 3;

    if (no_input) {
        std::cout << "Command help" << std::endl;
        return 1;
    }

    std::filesystem::path input_file = argv[2];

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
            bitmap.crop(0, 0, 100, 100).write(input_file.string());
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
            bitmap.resize(560, 560, Interpolation::NearestNeighbour).write(input_file.string());
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
    /* bool is_windows{false};

#if defined _WIN32 || defined _WIN64
    is_windows = true;
#endif

    GIF gif("bitmaps/sample_1.gif");
    gif.write("bitmaps/new.gif");
    return 0;

    for (std::string file : {"640x426", "1920x1280", "5184x3456"}) {
        std::string source = is_windows ? "bitmaps\\BMP_" + file + ".bmp" : "bitmaps/BMP_" + file + ".bmp";
        std::string destination = (is_windows ? "bitmaps\\test_" + file + "\\" : "bitmaps/test_" + file + "/");

        Bitmap bmp(source);
        if (file == "640x426") {
            bmp.crop(0, 0, 320, 213);
        } else if (file == "1920x1280") {
            bmp.crop(0, 0, 960, 640);
        } else {
            bmp.crop(0, 0, 2592, 1728);
        }
        bmp.write(destination + "test_crop.bmp");

        bmp.read(source);
        if (file == "640x426") {
            bmp.resize(800, 320, Interpolation::NearestNeighbour);
        } else if (file == "1920x1280") {
            bmp.resize(2400, 960, Interpolation::NearestNeighbour);
        } else {
            bmp.resize(6480, 2592, Interpolation::NearestNeighbour);
        }
        bmp.write(destination + "test_resize.bmp");

        bmp.read(source);
        bmp.flip().write(destination + "test_flip.bmp");

        bmp.read(source);
        bmp.rotate_clockwise().write(destination + "test_rotate.bmp");

        bmp.read(source);
        bmp.rotate_counterclockwise().write(destination + "test_rotate_cc.bmp");

        bmp.read(source);
        bmp.inverse().write(destination + "test_inverse.bmp");

        bmp.read(source).grayscale();
        bmp.write(destination + "test_grayscale.bmp");
    }

    return 0;*/
}

void act(Image& img) {

}