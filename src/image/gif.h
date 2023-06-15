#ifndef CIE_GIF
#define CIE_GIF

#include "image.h"

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

    std::vector<int> generate_code_stream();

    std::vector<std::string> generate_code_table();

   private:
    // Exactly 6 bytes
    struct Header {
        uint8_t signature[3]{0x47, 0x49, 0x46};
        uint8_t version[3]{0x38, 0x39, 0x61};  // GIF89a is default
    };

    // Exactly 7 bytes
    struct LogicalScreenDescriptor {
        uint8_t canvas_width[2]{0, 0};
        uint8_t canvas_height[2]{0, 0};
        uint8_t packed_field{0};
        uint8_t background_color_index{0};
        uint8_t pixel_aspect_ratio{0};
    } __attribute__((packed));

    struct GifImage {
        struct ImageDescriptor {
            uint8_t separator{0x2C};
            uint16_t image_left{0};
            uint16_t image_top{0};
            uint16_t image_width{0};
            uint16_t image_height{0};
            uint8_t packed_field{0};
        } __attribute__((packed));

        struct DataSubBlock {  // Continiously read these until 0 length is met
            uint8_t min_code_size{2};
            uint8_t length;
            std::vector<uint8_t> data;
            uint8_t trailer{0};
        };

        std::vector<uint8_t> full_pixel_data();

        ImageDescriptor image_descriptor;
        // std::set<Color> local_color_table; TODO: Local color table support
        uint8_t lzw_minimum_code_size;
        DataSubBlock pixel_data;
    };

    Header header;
    LogicalScreenDescriptor lsd;
    std::vector<Color> global_color_table;
    std::vector<GifImage> images;
    uint8_t trailer{0x3B};
};

#endif
