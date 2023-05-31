#ifndef IMAGE_LIB
#define IMAGE_LIB

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
        int32_t x_pixels_per_meter{0}; // INFO: Seems to be absolutely useless, but may be worth calculating later
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

struct GIF : Image {
   public:
    GIF(std::string file_name);

    GIF& read(std::string file_name);

    GIF& write(std::string file_name);

    GIF& set_pixel_data(std::vector<uint8_t> new_pixel_data) override;

    std::vector<uint8_t>& get_pixel_data() override;

    GIF& set_dimensions(uint32_t width, uint32_t height) override;

    uint32_t get_width() override;

    uint32_t get_height() override;

    uint32_t get_horizontal_resolution() override;

    uint32_t get_vertical_resolution() override;

    uint8_t get_channels() override;

    GIF& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) override;

    GIF& flip() override;

    GIF& rotate_clockwise() override;

    GIF& rotate_counterclockwise() override;

    GIF& grayscale() override;

    GIF& inverse() override;

    GIF& resize(uint32_t width, uint32_t height, Interpolation mode) override;

    GIF& set_pixel(uint32_t x, uint32_t y, Color color) override;

    Color get_pixel(uint32_t x, uint32_t y) override;

   private:
    struct Header {
        uint8_t signature[3] { 0x47, 0x49, 0x46 };
        uint8_t version[3] { 0x38, 0x39, 0x61 }; // GIF89a is default
    };

    struct LogicalScreenDescriptor {
        uint8_t canvas_width[2] { 0, 0 };
        uint8_t canvas_height[2] { 0, 0 };
        uint8_t packed_field {0};
        uint8_t background_color_index {0};
        uint8_t pixel_aspect_ratio {0};
    };

    struct GifImage {
        struct ImageDescriptor {
            uint8_t separator {0x2C};
            uint16_t image_left { 0 };
            uint16_t image_top { 0 };
            uint16_t image_width { 0 };
            uint16_t image_height { 0 };
            uint8_t packed_field {0};
        };

        struct DataSubBlock { // Continiously read these until 0 length is met
            uint8_t length;
            std::vector<uint8_t> data;
        };

        std::vector<uint8_t> full_pixel_data();

        ImageDescriptor image_descriptor;
        // std::set<Color> local_color_table; TODO: Local color table support
        uint8_t lzw_minimum_code_size;
        std::vector<DataSubBlock> pixel_data;
    };

    Header header;
    LogicalScreenDescriptor lsd;
    std::set<Color> global_color_table;
    std::vector<GifImage> images;
    uint8_t trailer {0x3B};
};

#endif
