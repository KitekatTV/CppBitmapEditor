#ifndef CIE_IMAGE
#define CIE_IMAGE

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <set>

#pragma region Misc

enum Interpolation {
    NearestNeighbour,
    Bilinear,
    Bicubic
};

struct Color {
    uint32_t raw();

    uint8_t R, G, B;

    Color(
        uint8_t R,
        uint8_t G,
        uint8_t B
    ) : R(R), G(G), B(B) {}

    Color() : R(0), G(0), B(0) {}
};

#pragma endregion

struct Image {
    Image() {};

   public:
    virtual Image& set_pixel_data(std::vector<uint8_t> new_pixel_data) = 0;

    virtual std::vector<uint8_t>& get_pixel_data() = 0;

    virtual Image& set_dimensions(uint32_t width, uint32_t height) = 0;

    virtual uint32_t get_width() = 0;

    virtual uint32_t get_height() = 0;

    virtual uint32_t get_horizontal_resolution() = 0;

    virtual uint32_t get_vertical_resolution() = 0;

    virtual uint8_t get_channels() = 0;

    virtual Image& set_pixel(uint32_t x, uint32_t y, Color color) = 0;

    virtual Color get_pixel(uint32_t x, uint32_t y) = 0;

    virtual Image& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height);

    virtual Image& flip();

    virtual Image& rotate_clockwise();

    virtual Image& rotate_counterclockwise();

    virtual Image& grayscale();

    virtual Image& inverse();

    virtual Image& resize(uint32_t width, uint32_t height, Interpolation mode);
};

#endif