#ifndef CIE_BITMAP
#define CIE_BITMAP

#include "image.h"

/// @struct Bitmap
/// @brief Class representing a bitmap picture (Microsoft BMP)
struct Bitmap : Image {
   public:
    /// Main constructor. Implicitly calls Bitmap::read(std::string)
    Bitmap(std::string file_name);

    /// @brief Read data from .bmp file to this instance.
    /// @param file_name Name of the file to read from.
    /// @return The reference to this bitmap.
    Bitmap& read(std::string file_name);

    /// @brief Write current data to the target file.
    /// @param file_name Name of the file to write to.
    /// @return The reference to this bitmap.
    Bitmap& write(std::string file_name);

    /// @brief Replace current vector containing pixel data with a new one.
    /// @param new_pixel_data New vector of bytes.
    /// @return The reference to this bitmap.
    Bitmap& set_pixel_data(std::vector<uint8_t> new_pixel_data) override;

    /// @brief Get current pixel data.
    /// @return Vector of pixel values as bytes.
    std::vector<uint8_t>& get_pixel_data() override;

    /// @brief Change bitmap dimensions.
    /// @param width New width.
    /// @param height New height.
    /// @return The reference to this bitmap.
    Bitmap& set_dimensions(uint32_t width, uint32_t height) override;

    /// @brief Get current bitmap width.
    /// @return Width of the bitmap as unsigned int.
    uint32_t get_width() override;

    /// @brief Get current bitmap height.
    /// @return Height of the bitmap as unsigned int.
    uint32_t get_height() override;

    /// @brief Get horizontal resolution current bitmap.
    /// @return Horizontal resolution of the bitmap (in pixels per meter).
    uint32_t get_horizontal_resolution() override;

    /// @brief Get vertical resolution current bitmap.
    /// @return Vertical resolution of the bitmap (in pixels per meter).
    uint32_t get_vertical_resolution() override;

    /// @brief Get number of channels of current bitmap.
    /// @return Number of channels.
    uint8_t get_channels() override;

    /// @brief Crop the bitmap.
    /// @param x0 Vertical starting point.
    /// @param y0 Horizontal starting point.
    /// @param width Width of target bitmap.
    /// @param height height of target bitmap.
    /// @return The reference to this bitmap.
    Bitmap& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) override;

    /// @brief Flip the bitmap.
    /// @return The reference to this bitmap.
    Bitmap& flip() override;

    /// @brief Rotate the bitmap clockwise (90 degrees).
    /// @return The reference to this bitmap.
    Bitmap& rotate_clockwise() override;

    /// @brief Rotate the bitmap counterclockwise (270 degrees).
    /// @return The reference to this bitmap.
    Bitmap& rotate_counterclockwise() override;

    /// @brief Convert the bitmap to grayscale. Does not change the color depth.
    /// @return The reference to this bitmap.
    Bitmap& grayscale() override;

    /// @brief Invert the colors of this bitmap.
    /// @return The reference to this bitmap.
    Bitmap& inverse() override;

    /// @brief Resize the bitmap.
    /// @param width Width of target bitmap.
    /// @param height height of target bitmap.
    /// @param mode Interpolation type.
    /// @return The reference to this bitmap.
    Bitmap& resize(uint32_t width, uint32_t height, Interpolation mode) override;

    /// @brief Change value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @param color New pixel value.
    /// @return The reference to this bitmap.
    Bitmap& set_pixel(uint32_t x, uint32_t y, Color color) override;

    /// @brief Get value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @return RGB value of pixel as Color.
    Color get_pixel(uint32_t x, uint32_t y) override;

   private:
    /// @struct Bitmap header
    /// @brief Stores general information about the bitmap image file.
    struct Header {
        uint16_t signature{0x4D42}; /// Bitmap signature. All bmp file have these bytes at the beginning.
        uint32_t file_size{0}; /// Size of file in bytes.
        uint32_t reserved{0}; /// Reserved byte. Unused.
        uint32_t data_offset{0}; /// File offset to raster data.
    } __attribute__((packed));

    /// @struct Bitmap info header
    /// @brief Stores detailed information about the bitmap image and defines the pixel format.
    struct InfoHeader {
        uint32_t header_size{0}; /// Size of the info header in bytes. Must always be 40.
        int32_t image_width{0}; /// Bitmap width.
        int32_t image_height{0}; /// Bitmap height.
        uint16_t planes{0}; /// Number of planes. Always 1.
        uint16_t bits_per_pixel{0}; /// Image depth.
        uint32_t compression{0}; /// Type of compression. Not used, always 0.
        uint32_t image_size{0}; /// Size of compressed image. Since compression is always zero, this is 0 too.
        int32_t x_pixels_per_meter{0}; /// Image horizontal resolution. Unused.
        int32_t y_pixels_per_meter{0}; /// Image horizontal resolution. Unused.
        uint32_t colors_used{0}; /// Number of actually used colors. Unused.
        uint32_t colors_important{0}; /// Number of important colors. Unused.
    };

    /// @struct Color data
    /// @brief Used to define colors used by the bitmap image data. Mandatory for color depths <= 8 bits.
    struct ColorData {
        uint8_t red_intensity{0};
        uint8_t green_intensity{0};
        uint8_t blue_intensity{0};
        uint8_t reserved{0};
    };

    Header header; /// Current bitmap header
    InfoHeader info_header; /// Current bitmap info header
    std::vector<ColorData> color_table; /// Current bitmap color table
    std::vector<uint8_t> pixel_data; /// Current bitmap pixel data (as bytes)
};

#endif