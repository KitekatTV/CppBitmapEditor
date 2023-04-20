#ifndef BITMAP
#define BITMAP

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

enum Interpolation {
    NearestNeighbour,
    Bilinear,
};

struct Color {
    uint32_t raw();

    uint8_t R, G, B, A;

    Color(
        uint8_t R,
        uint8_t G,
        uint8_t B,
        uint8_t A = 0)
        : R(R), G(G), B(B), A(A) {}

    Color() : R(0), G(0), B(0), A(0) {}
};

struct Bitmap {
   public:
    Bitmap(std::string file_name);

    Bitmap& read(std::string file_name);

    Bitmap& write(std::string file_name);

    Bitmap& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height);

    Bitmap& flip();

    Bitmap& rotate_clockwise();

    Bitmap& rotate_counterclockwise();

    Bitmap& grayscale();

    Bitmap& inverse();

    Bitmap& resize(uint32_t width, uint32_t height, Interpolation mode);

    Bitmap& set_pixel(uint32_t x, uint32_t y, Color color, uint8_t channels = 3);

    Color get_pixel(uint32_t x, uint32_t y, uint8_t channels = 3);

   private:
    struct Header {
        uint16_t signature{0x4D42};
        uint32_t file_size{0};
        uint32_t reserved{0};
        uint32_t data_offset{0};
    } __attribute__((packed));

    struct InfoHeader {
        uint32_t header_size{0};
        int32_t image_width{0};
        int32_t image_height{0};
        uint16_t planes{0};
        uint16_t bits_per_pixel{0};
        uint32_t compression{0};
        uint32_t image_size{0};
        int32_t x_pixels_per_meter{0};
        int32_t y_pixels_per_meter{0};
        uint32_t colors_used{0};
        uint32_t colors_important{0};
    };

    struct ColorData {
        uint8_t red_intensity{0};
        uint8_t green_intensity{0};
        uint8_t blue_intensity{0};
        uint8_t reserved{0};
    };

    Header header;
    InfoHeader info_header;
    std::vector<ColorData> color_table;
    std::vector<uint8_t> pixel_data;
};

#endif
