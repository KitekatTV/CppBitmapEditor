#ifndef CIE_IMAGE
#define CIE_IMAGE

#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <set>

#pragma region Misc

/// Enum with different interpolation methods.
enum Interpolation {
    NearestNeighbour, /// Nearest-neighbour interpolation (Default).
    Bilinear, /// Bilinear interpolation.
    Bicubic /// Bicubic interpolation. Slow and generally not recommended.
};

/// @struct Color
/// @brief Simple class for storing colors as RGB byte tuple.
struct Color {
    /// Method to concat color to 32-bit uint. Currently unused.
    /// @return 32bit representation of color.
    uint32_t raw();

    uint8_t R, G, B; /// Color RGB values as bytes.

    /// Basic constructor.
    Color(
        uint8_t R,
        uint8_t G,
        uint8_t B
    ) : R(R), G(G), B(B) {}

    /// Default constructor.
    Color() : R(0), G(0), B(0) {}
};

#pragma endregion

/// @struct Image
/// @brief Abstract class acting as base for all other formats.
struct Image {
    /// Default constructor.
    Image() {};

   public:
    /// @brief Replace current vector containing pixel data with a new one.
    /// @param new_pixel_data New vector of bytes.
    /// @return The reference to this image.
    virtual Image& set_pixel_data(std::vector<uint8_t> new_pixel_data) = 0;

    /// @brief Get current pixel data.
    /// @return Vector of pixel values as bytes.
    virtual std::vector<uint8_t>& get_pixel_data() = 0;

    /// @brief Change image dimensions.
    /// @param width New width.
    /// @param height New height.
    /// @return The reference to this image.
    virtual Image& set_dimensions(uint32_t width, uint32_t height) = 0;

    /// @brief Get current image width.
    /// @return Width of the image as unsigned int.
    virtual uint32_t get_width() = 0;

    /// @brief Get current image height.
    /// @return Height of the image as unsigned int.
    virtual uint32_t get_height() = 0;

    /// @brief Get horizontal resolution current image.
    /// @return Horizontal resolution of the image (in pixels per meter).
    virtual uint32_t get_horizontal_resolution() = 0;

    /// @brief Get vertical resolution current image.
    /// @return Vertical resolution of the image (in pixels per meter).
    virtual uint32_t get_vertical_resolution() = 0;

    /// @brief Get number of channels of current image.
    /// @return Number of channels.
    virtual uint8_t get_channels() = 0;

    /// @brief Change value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @param color New pixel value.
    /// @return The reference to this image.
    virtual Image& set_pixel(uint32_t x, uint32_t y, Color color) = 0;

    /// @brief Get value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @return RGB value of pixel as Color.
    virtual Color get_pixel(uint32_t x, uint32_t y) = 0;

    /// @brief Crop the image.
    /// @param x0 Vertical starting point.
    /// @param y0 Horizontal starting point.
    /// @param width Width of target image.
    /// @param height height of target image.
    /// @return The reference to this image.
    virtual Image& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height);

    /// @brief Flip the image.
    /// @return The reference to this image.
    virtual Image& flip();

    /// @brief Rotate the image clockwise (90 degrees).
    /// @return The reference to this image.
    virtual Image& rotate_clockwise();

    /// @brief Rotate the image counterclockwise (270 degrees).
    /// @return The reference to this image.
    virtual Image& rotate_counterclockwise();

    /// @brief Convert the image to grayscale. Does not change the color depth.
    /// @return The reference to this image.
    virtual Image& grayscale();

    /// @brief Invert the colors of this image.
    /// @return The reference to this image.
    virtual Image& inverse();

    /// @brief Resize the image.
    /// @param width Width of target image.
    /// @param height height of target image.
    /// @param mode Interpolation type.
    /// @return The reference to this image.
    virtual Image& resize(uint32_t width, uint32_t height, Interpolation mode);
};

#endif