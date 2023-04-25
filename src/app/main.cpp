#include <iostream>
#include <string>

#include "image.h"

int main() {
    bool is_windows{false};

#if defined _WIN32 || defined _WIN64
    is_windows = true;
#endif

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

    return 0;
}
