#include "bitmap.h"

Bitmap::Bitmap(std::string file_name) {
    read(file_name);
}

Bitmap& Bitmap::read(std::string file_name) {
    std::ifstream stream(file_name, std::ios::binary);

    if (stream) {
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.signature != 0x4D42)
            throw file_name + " is not a valid .bmp bitmap file!";

        stream.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

        stream.seekg(header.data_offset, stream.beg);

        info_header.image_size = sizeof(info_header);
        header.data_offset = sizeof(info_header) + sizeof(header);
        header.file_size = header.data_offset;

        pixel_data.resize(info_header.image_height * info_header.image_width * info_header.bits_per_pixel / 8);

        if (info_header.image_width % 4 == 0) {
            stream.read(reinterpret_cast<char*>(pixel_data.data()), pixel_data.size());
            header.file_size += pixel_data.size();
        } else {
            unsigned int row_length = info_header.image_width * info_header.bits_per_pixel / 8;
            unsigned int new_row_length = row_length + 4 - (row_length % 4);
            std::vector<uint8_t> padding(new_row_length - row_length);
            for (int y = 0; y < info_header.image_height; ++y) {
                stream.read(reinterpret_cast<char*>(pixel_data.data() + row_length * y), row_length);
                stream.read(reinterpret_cast<char*>(padding.data()), padding.size());
            }
            header.file_size += pixel_data.size() + info_header.image_height * padding.size();
        }
    }

    return *this;
}

Bitmap& Bitmap::write(std::string file_name) {
    std::ofstream stream(file_name, std::ios::binary);

    if (info_header.image_width % 4 == 0) {
        stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));
        stream.write(reinterpret_cast<const char*>(pixel_data.data()), pixel_data.size());
    } else {
        unsigned int row_length = info_header.image_width * info_header.bits_per_pixel / 8;
        unsigned int new_row_length = row_length + 4 - (row_length % 4);
        std::vector<uint8_t> padding(new_row_length - row_length);

        stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
        stream.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

        for (int y = 0; y < info_header.image_height; ++y) {
            stream.write(reinterpret_cast<const char*>(pixel_data.data() + row_length * y), row_length);
            stream.write(reinterpret_cast<const char*>(padding.data()), padding.size());
        }
    }

    return *this;
}

uint8_t Bitmap::get_channels() { return info_header.bits_per_pixel / 8; }

uint32_t Bitmap::get_width() { return info_header.image_width; }

uint32_t Bitmap::get_height() { return info_header.image_height; }

uint32_t Bitmap::get_horizontal_resolution() { return info_header.x_pixels_per_meter; }

uint32_t Bitmap::get_vertical_resolution() { return info_header.y_pixels_per_meter; }

std::vector<uint8_t>& Bitmap::get_pixel_data() { return pixel_data; }

Bitmap& Bitmap::set_dimensions(uint32_t width, uint32_t height) {
    info_header.image_width = width;
    info_header.image_height = height;
    return *this;
}

Bitmap& Bitmap::set_pixel_data(std::vector<uint8_t> new_pixel_data) {
    pixel_data = std::move(new_pixel_data);
    return *this;
}

Bitmap& Bitmap::crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) {
    Image::crop(x0, y0, width, height);
    return *this;
};

Bitmap& Bitmap::flip() {
    Image::flip();
    return *this;
};

Bitmap& Bitmap::rotate_clockwise() {
    Image::rotate_clockwise();
    return *this;
};

Bitmap& Bitmap::rotate_counterclockwise() {
    Image::rotate_counterclockwise();
    return *this;
};

Bitmap& Bitmap::grayscale() {
    Image::grayscale();
    return *this;
};

Bitmap& Bitmap::inverse() {
    Image::inverse();
    return *this;
};

Bitmap& Bitmap::resize(uint32_t width, uint32_t height, Interpolation mode) {
    Image::resize(width, height, mode);
    return *this;
};

Color Bitmap::get_pixel(uint32_t x, uint32_t y) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t>& pixel_data = get_pixel_data();

    uint8_t B = pixel_data[current_pixel + 0];
    uint8_t G = pixel_data[current_pixel + 1];
    uint8_t R = pixel_data[current_pixel + 2];

    return Color(R, G, B);
}

Bitmap& Bitmap::set_pixel(uint32_t x, uint32_t y, Color color) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t>& pixel_data = get_pixel_data();

    pixel_data[current_pixel + 0] = color.B;
    pixel_data[current_pixel + 1] = color.G;
    pixel_data[current_pixel + 2] = color.R;

    return *this;
}