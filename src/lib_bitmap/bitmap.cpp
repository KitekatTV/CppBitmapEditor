#include "bitmap.h"

#include <algorithm>
#include <cmath>
#include <type_traits>

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

Bitmap& Bitmap::crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) {
    if (x0 + width > info_header.image_width || y0 + height > info_header.image_height)
        throw;

    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;
    std::vector<uint8_t> new_pixel_data;

    for (uint32_t y = y0; y < y0 + height; ++y) {
        for (uint32_t x = x0; x < x0 + width; ++x) {
            Color current_pixel = get_pixel(x, y, channels);
            new_pixel_data.push_back(current_pixel.B);
            new_pixel_data.push_back(current_pixel.G);
            new_pixel_data.push_back(current_pixel.R);

            if (channels == 4)
                new_pixel_data.push_back(current_pixel.A);
        }
    }

    info_header.image_width = width;
    info_header.image_height = height;
    info_header.image_size = sizeof(info_header);

    pixel_data = new_pixel_data;
    header.data_offset = sizeof(info_header) + sizeof(header);
    header.file_size = header.data_offset + pixel_data.size();

    return *this;
}

Bitmap& Bitmap::flip() {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;

    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(pixel_data.size());

    for (uint32_t y = 0; y < info_header.image_height; ++y) {
        for (uint32_t x = 0; x < info_header.image_width; ++x) {
            uint32_t source_pixel = channels * (y * info_header.image_width + x);
            uint32_t destination_pixel = channels * ((info_header.image_height - y - 1) * info_header.image_width + x);

            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];

            if (channels == 4)
                new_pixel_data[destination_pixel + 3] = pixel_data[source_pixel + 3];
        }
    }

    pixel_data = new_pixel_data;

    return *this;
}

Bitmap& Bitmap::rotate_clockwise() {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;

    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(pixel_data.size());

    for (uint32_t y = 0; y < info_header.image_height; ++y) {
        for (uint32_t x = 0; x < info_header.image_width; ++x) {
            uint32_t source_pixel = channels * (y * info_header.image_width + x);
            uint32_t destination_pixel = channels * ((info_header.image_height - y - 1) + info_header.image_height * x);

            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];

            if (channels == 4)
                new_pixel_data[destination_pixel + 3] = pixel_data[source_pixel + 3];
        }
    }

    pixel_data = new_pixel_data;
    std::swap(info_header.image_width, info_header.image_height);
    std::swap(info_header.x_pixels_per_meter, info_header.y_pixels_per_meter);

    return *this;
}

Bitmap& Bitmap::rotate_counterclockwise() {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;

    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(pixel_data.size());

    for (uint32_t y = 0; y < info_header.image_height; ++y) {
        for (uint32_t x = 0; x < info_header.image_width; ++x) {
            uint32_t source_pixel = channels * (y * info_header.image_width + x);
            uint32_t destination_pixel = channels * (y + info_header.image_height * x);

            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];

            if (channels == 4)
                new_pixel_data[destination_pixel + 3] = pixel_data[source_pixel + 3];
        }
    }

    pixel_data = new_pixel_data;
    std::swap(info_header.image_width, info_header.image_height);
    std::swap(info_header.x_pixels_per_meter, info_header.y_pixels_per_meter);

    return *this;
}

Bitmap& Bitmap::grayscale() {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;
    for (uint32_t y = 0; y < info_header.image_height; ++y) {
        for (uint32_t x = 0; x < info_header.image_width; ++x) {
            Color color = get_pixel(x, y, channels);
            uint32_t grayscale = color.R * .299 + color.G * .587 + color.B * .144;
            set_pixel(x, y, Color(grayscale, grayscale, grayscale), channels);
        }
    }

    return *this;
}

Bitmap& Bitmap::inverse() {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;
    for (uint32_t y = 0; y < info_header.image_height; ++y) {
        for (uint32_t x = 0; x < info_header.image_width; ++x) {
            Color color = get_pixel(x, y, channels);
            set_pixel(x, y, Color(0xFF - color.R, 0xFF - color.G, 0xFF - color.B), channels);
        }
    }

    return *this;
}

Bitmap& Bitmap::resize(uint32_t width, uint32_t height, Interpolation mode) {
    uint32_t channels = info_header.bits_per_pixel == 32 ? 4 : 3;

    std::vector<uint8_t> new_pixel_data;

    float x_ratio, y_ratio;
    switch (mode) {
        case Interpolation::NearestNeighbour:
            x_ratio = static_cast<float>(width) / static_cast<float>(info_header.image_width);
            y_ratio = static_cast<float>(height) / static_cast<float>(info_header.image_height);
            break;
        case Interpolation::Bilinear:
            x_ratio = static_cast<float>(info_header.image_width - 1) / static_cast<float>(width); // NOTE: Using these ratios for NN interpolation gives an interesting result
            y_ratio = static_cast<float>(info_header.image_height - 1) / static_cast<float>(height);
            break;
    }

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            switch (mode) {
                case Interpolation::NearestNeighbour: {
                    uint32_t source_x = std::round(x / x_ratio);
                    uint32_t source_y = std::round(y / y_ratio);

                    uint32_t source_pixel = channels * (source_y * info_header.image_width + source_x);

                    new_pixel_data.push_back(pixel_data[source_pixel + 0]);
                    new_pixel_data.push_back(pixel_data[source_pixel + 1]);
                    new_pixel_data.push_back(pixel_data[source_pixel + 2]);

                    if (channels == 4)
                        new_pixel_data.push_back(pixel_data[source_pixel + 3]);

                    break;
                }

                case Interpolation::Bilinear: {
                    uint32_t source_x = x_ratio * x;
                    uint32_t source_y = y_ratio * y;

                    float x_diff = (x_ratio * x) - source_x;
                    float y_diff = (y_ratio * y) - source_y;

                    Color A = get_pixel(source_x, source_y, channels);
                    Color B = get_pixel(source_x + 1, source_y, channels);
                    Color C = get_pixel(source_x, source_y + 1, channels);
                    Color D = get_pixel(source_x + 1, source_y + 1, channels);

                    float blue = A.B * (1 - x_diff) * (1 - y_diff) + B.B * x_diff * (1 - y_diff) + C.B * y_diff * (1 - x_diff) + D.B * (x_diff * y_diff);
                    float green = A.G * (1 - x_diff) * (1 - y_diff) + B.G * x_diff * (1 - y_diff) + C.G * y_diff * (1 - x_diff) + D.G * (x_diff * y_diff);
                    float red = A.R * (1 - x_diff) * (1 - y_diff) + B.R * x_diff * (1 - y_diff) + C.R * y_diff * (1 - x_diff) + D.R * (x_diff * y_diff);

                    new_pixel_data.push_back(static_cast<uint8_t>(blue));
                    new_pixel_data.push_back(static_cast<uint8_t>(green));
                    new_pixel_data.push_back(static_cast<uint8_t>(red));
                    break;
                }
            }
        }
    }

    info_header.image_width = width;
    info_header.image_height = height;

    pixel_data = new_pixel_data;
    header.data_offset = sizeof(info_header) + sizeof(header);
    header.file_size = header.data_offset + pixel_data.size();

    return *this;
}

Color Bitmap::get_pixel(uint32_t x, uint32_t y, uint8_t channels) {
    uint32_t current_pixel = channels * (y * info_header.image_width + x);

    uint8_t B = pixel_data[current_pixel + 0];
    uint8_t G = pixel_data[current_pixel + 1];
    uint8_t R = pixel_data[current_pixel + 2];
    uint8_t A = channels == 4 ? pixel_data[current_pixel + 3] : 0;

    return Color(R, G, B, A);
}

Bitmap& Bitmap::set_pixel(uint32_t x, uint32_t y, Color color, uint8_t channels) {
    uint32_t current_pixel = channels * (y * info_header.image_width + x);

    pixel_data[current_pixel + 0] = color.B;
    pixel_data[current_pixel + 1] = color.G;
    pixel_data[current_pixel + 2] = color.R;

    if (channels == 4)
        pixel_data[current_pixel + 3] = color.A;

    return *this;
}

uint32_t Color::raw() {
    return static_cast<uint32_t>(R) << 16 | (static_cast<uint32_t>(G) << 8 | static_cast<uint32_t>(B));
}
