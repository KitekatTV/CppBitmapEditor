#ifndef CIE_BITMAP
#define CIE_BITMAP

#include "image.h"

struct Bitmap : Image {
   public:
    Bitmap(std::string file_name);

    Bitmap& read(std::string file_name);

    Bitmap& write(std::string file_name);

    Bitmap& set_pixel_data(std::vector<uint8_t> new_pixel_data) override;

    std::vector<uint8_t>& get_pixel_data() override;

    Bitmap& set_dimensions(uint32_t width, uint32_t height) override;

    uint32_t get_width() override;

    uint32_t get_height() override;

    uint32_t get_horizontal_resolution() override;

    uint32_t get_vertical_resolution() override;

    uint8_t get_channels() override;

    Bitmap& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) override;

    Bitmap& flip() override;

    Bitmap& rotate_clockwise() override;

    Bitmap& rotate_counterclockwise() override;

    Bitmap& grayscale() override;

    Bitmap& inverse() override;

    Bitmap& resize(uint32_t width, uint32_t height, Interpolation mode) override;

    Bitmap& set_pixel(uint32_t x, uint32_t y, Color color) override;

    Color get_pixel(uint32_t x, uint32_t y) override;

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
        int32_t x_pixels_per_meter{0};  // INFO: Seems to be absolutely useless, but may be worth calculating later
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