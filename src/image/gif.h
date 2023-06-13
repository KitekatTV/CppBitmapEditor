#ifndef CIE_GIF
#define CIE_GIF

#include "image.h"

/// @struct GIF
/// @brief Class representing a gif picture (Graphics Interchange Format)
/// Currently does not implement animation and transparency (supports only GIF87)
struct GIF : Image {
   public:
    /// Main constructor. Implicitly calls GIF::read(std::string)
    GIF(std::string file_name);

    /// @brief Read data from .gif file to this instance.
    /// @param file_name Name of the file to read from.
    /// @return The reference to this image.
    GIF& read(std::string file_name);

    /// @brief Write current data to the target file.
    /// @param file_name Name of the file to write to.
    /// @return The reference to this image.
    GIF& write(std::string file_name);

    /// @brief Replace current vector containing pixel data with a new one.
    /// @param new_pixel_data New vector of bytes.
    /// @return The reference to this image.
    GIF& set_pixel_data(std::vector<uint8_t> new_pixel_data) override;

    /// @brief Get current pixel data.
    /// @return Vector of pixel values as bytes.
    std::vector<uint8_t>& get_pixel_data() override;

    /// @brief Change image dimensions.
    /// @param width New width.
    /// @param height New height.
    /// @return The reference to this image.
    GIF& set_dimensions(uint32_t width, uint32_t height) override;

    /// @brief Get current image width.
    /// @return Width of the image as unsigned int.
    uint32_t get_width() override;

    /// @brief Get current image height.
    /// @return Height of the image as unsigned int.
    uint32_t get_height() override;

    /// @brief Get horizontal resolution current image.
    /// @return Horizontal resolution of the image (in pixels per meter).
    uint32_t get_horizontal_resolution() override;

    /// @brief Get vertical resolution current image.
    /// @return Vertical resolution of the image (in pixels per meter).
    uint32_t get_vertical_resolution() override;

    /// @brief Get number of channels of current image.
    /// @return Number of channels.
    uint8_t get_channels() override;

    /// @brief Crop the image.
    /// @param x0 Vertical starting point.
    /// @param y0 Horizontal starting point.
    /// @param width Width of target image.
    /// @param height height of target image.
    /// @return The reference to this image.
    GIF& crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) override;

    /// @brief Flip the image.
    /// @return The reference to this image.
    GIF& flip() override;

    /// @brief Rotate the image clockwise (90 degrees).
    /// @return The reference to this image.
    GIF& rotate_clockwise() override;

    /// @brief Rotate the image counterclockwise (270 degrees).
    /// @return The reference to this image.
    GIF& rotate_counterclockwise() override;

    /// @brief Convert the image to grayscale. Does not change the color depth.
    /// @return The reference to this image.
    GIF& grayscale() override;

    /// @brief Invert the colors of this image.
    /// @return The reference to this image.
    GIF& inverse() override;

    /// @brief Resize the image.
    /// @param width Width of target image.
    /// @param height height of target image.
    /// @param mode Interpolation type.
    /// @return The reference to this image.
    GIF& resize(uint32_t width, uint32_t height, Interpolation mode) override;

    /// @brief Change value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @param color New pixel value.
    /// @return The reference to this image.
    GIF& set_pixel(uint32_t x, uint32_t y, Color color) override;

    /// @brief Get value of pixel at given coordinates.
    /// @param x X coordinate of target pixel.
    /// @param y Y coordinate of target pixel.
    /// @return RGB value of pixel as Color.
    Color get_pixel(uint32_t x, uint32_t y) override;

   private:
    /// @struct GIF header
    /// @brief All GIF files must start with a header block.
    struct Header {
        uint8_t signature[3]{0x47, 0x49, 0x46}; /// GIF signature.
        uint8_t version[3]{0x38, 0x39, 0x61};  /// GIF version (87a or 89a).
    };

    /// @struct Logical Screen Descriptor
    /// @brief This block tells the decoder how much room this image will take up. It is exactly seven bytes long.
    struct LogicalScreenDescriptor {
        uint8_t canvas_width[2]{0, 0}; /// Used to specified canvas width. Not used nowadays.
        uint8_t canvas_height[2]{0, 0};  /// Used to specified canvas height. Not used nowadays.
        uint8_t packed_field{0}; /// Contains four fields of packed data.
        uint8_t background_color_index{0}; /// The index of GCT color to use as background. Not used nowadays.
        uint8_t pixel_aspect_ratio{0}; /// Pixel aspect ratio. Not used nowadays.
    } __attribute__((packed));

    /// @struct Actual GIF image
    /// @brief Contains data of the actual image.
    struct GifImage {
        /// @struct Image descriptor
        /// @brief Each image begins with an image descriptor block. This block is exactly 10 bytes long.
        struct ImageDescriptor {
            uint8_t separator{0x2C}; /// Every image descriptor begins with the value 2C.
            uint16_t image_left{0}; /// Image left position. Not used nowadays.
            uint16_t image_top{0}; /// Image right position. Not used nowadays.
            uint16_t image_width{0}; /// How many space image takes on the canvas. Not used nowadays.
            uint16_t image_height{0}; /// How many space image takes on the canvas. Not used nowadays.
            uint8_t packed_field{0}; /// Contains five fields of packed data.
        } __attribute__((packed));

        /// @struct Data sub-block
        /// @brief The image data is composed of a series of output codes which tell the decoder which colors to emit to the canvas.
        struct DataSubBlock {  // Continiously read these until 0 length is met
            uint8_t length; /// How many bytes of actual data follow.
            std::vector<uint8_t> data; /// The data itself.
        };

        std::vector<uint8_t> full_pixel_data();

        ImageDescriptor image_descriptor; /// Current image descriptor.
        // std::set<Color> local_color_table; TODO: Local color table support
        uint8_t lzw_minimum_code_size; // Move to sub-block
        std::vector<DataSubBlock> pixel_data; // Move sub-block from struct fo fields?
    };

    Header header; /// Current image header.
    LogicalScreenDescriptor lsd; /// Current image loginal screen descriptor.
    std::set<Color> global_color_table; // Not needed to keep?
    std::vector<GifImage> images; /// All the stored images.
    uint8_t trailer{0x3B}; /// Trailer.
};

#endif