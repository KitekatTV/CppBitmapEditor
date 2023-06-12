#include "image.h"

#include <algorithm>
#include <cmath>
#include <iterator>
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

uint8_t Bitmap::get_channels() { return info_header.bits_per_pixel / 8; }

uint32_t Bitmap::get_width() { return info_header.image_width; }

uint32_t Bitmap::get_height() { return info_header.image_height; }

uint32_t Bitmap::get_horizontal_resolution() { return info_header.x_pixels_per_meter; }

uint32_t Bitmap::get_vertical_resolution() { return info_header.y_pixels_per_meter; }

std::vector<uint8_t> & Bitmap::get_pixel_data() { return pixel_data; }

Bitmap& Bitmap::set_dimensions(uint32_t width, uint32_t height) { info_header.image_width = width; info_header.image_height = height; return *this; }

Bitmap& Bitmap::set_pixel_data(std::vector<uint8_t> new_pixel_data) { pixel_data = std::move(new_pixel_data); return *this; }

Bitmap& Bitmap::crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) { Image::crop(x0, y0, width, height); return *this; };

Bitmap& Bitmap::flip() { Image::flip(); return *this; };

Bitmap& Bitmap::rotate_clockwise() { Image::rotate_clockwise(); return *this; };

Bitmap& Bitmap::rotate_counterclockwise() { Image::rotate_counterclockwise(); return *this; };

Bitmap& Bitmap::grayscale() { Image::grayscale(); return *this; };

Bitmap& Bitmap::inverse() { Image::inverse(); return *this; };

Bitmap& Bitmap::resize(uint32_t width, uint32_t height, Interpolation mode) { Image::resize(width, height, mode); return *this; };

Color Bitmap::get_pixel(uint32_t x, uint32_t y) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t> & pixel_data = get_pixel_data();

    uint8_t B = pixel_data[current_pixel + 0];
    uint8_t G = pixel_data[current_pixel + 1];
    uint8_t R = pixel_data[current_pixel + 2];

    return Color(R, G, B);
}

Bitmap& Bitmap::set_pixel(uint32_t x, uint32_t y, Color color) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t> & pixel_data = get_pixel_data();

    pixel_data[current_pixel + 0] = color.B;
    pixel_data[current_pixel + 1] = color.G;
    pixel_data[current_pixel + 2] = color.R;

    return *this;
}

GIF& GIF::read(std::string file_name) {
    std::ifstream stream(file_name, std::ios::binary);

    if (stream) {
        // Header
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        uint8_t signature[3] { 0x47, 0x49, 0x46 };
        if (header.signature != signature)
            throw file_name + " is not a valid GIF file!";

        // LogicalScreenDescriptor
        stream.read(reinterpret_cast<char*>(&lsd), sizeof(lsd));

        // Global color table
        bool use_global_color_table = ((lsd.packed_field >> 7) & 1) == 1 ? true : false;

        if (use_global_color_table) {
            uint8_t table_size = lsd.packed_field & 0b111;
            for (int i = 0; i < std::pow(2, table_size + 1); ++i) {
                Color current_color;
                for (int j = 0; j < 3; ++j) {
                    uint8_t current_color_channel = 0;
                    stream.read(reinterpret_cast<char*>(&current_color_channel), sizeof(current_color_channel));
                    switch (j) {
                        case 0:
                            current_color.R = current_color_channel;
                            break;

                        case 1:
                            current_color.G = current_color_channel;
                            break;

                        case 2:
                            current_color.B = current_color_channel;
                            break;
                    }
                }

                global_color_table.insert(current_color);
            }
        }

        // Images
    }

    return *this;
}

std::vector<uint8_t> GIF::GifImage::full_pixel_data() {
    std::vector<uint8_t> data = pixel_data[0].data;

    for (int i = 1; i < pixel_data.size(); ++i)
        data.insert(data.end(), pixel_data[i].data.begin(), pixel_data[i].data.end());

    return data;
}

GIF& GIF::write(std::string file_name) {
    std::ofstream stream(file_name, std::ios::binary);

    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(reinterpret_cast<const char*>(&lsd), sizeof(lsd));

    if (((lsd.packed_field >> 7) & 0x1) == 0x1)
        for (Color color : global_color_table)
            stream.write(reinterpret_cast<const char*>(color.raw()), sizeof(uint8_t) * 3);
    else
        throw;

    for (GifImage current_image : images) {
        stream.write(reinterpret_cast<const char*>(&current_image.image_descriptor), sizeof(current_image.image_descriptor));
        std::set<std::string> code_table; // INFO: Special table for storing sequences of colours
        std::vector<std::string> index_stream; // INFO: Input as color indexes from color table
        std::vector<int> code_stream; // INFO: Output as codes from code table
        std::string index_buffer; // INFO: Buffer for color indexes

        std::vector<uint8_t> current_data = current_image.full_pixel_data();
        for (uint8_t pixel : current_data)
            index_stream.push_back(std::to_string(pixel));

        // STEP 1: Initialize code table
        // if (((current_image.image_descriptor.packed_field >> 7) & 0x1) == 0x1)
        //     for (uint32_t i = 0; i < current_image.local_color_table.size(); ++i)
        //         code_table.insert(std::to_string(i));
        if (((lsd.packed_field >> 7) & 0x1) == 0x1)
            for (uint32_t i = 0; i < global_color_table.size(); ++i)
                code_table.insert(std::to_string(i));
        else
            throw;

        code_table.insert("CC");
        code_table.insert("EOI");

        // STEP 2: Send clear code to the code stream
        code_stream.push_back(std::distance(code_table.begin(), code_table.find("CC")));

        // STEP 3: Read first value from index stream to buffer
        index_buffer = index_stream[0];

        // LOOP
        int index = 1;
        while (index < index_stream.size()) {
            std::string K = index_stream[index]; // Get next index from index stream (K)

            if (code_table.find(index_buffer + "," + K) != code_table.end()) {
                index_buffer += "," + K; // Add K to the index buffer
            } else {
                code_table.insert(index_buffer + "," + K); // Add a new row for index buffer + K to the code table
                code_stream.push_back(std::distance(code_table.begin(), code_table.find(index_buffer))); // Output code for just the buffer to the code stream
                index_buffer = K; // Set index buffer to K
                // Set K to nothing (Not required since K is local to cycle)
            }

            index++;
        }

        code_stream.push_back(std::distance(code_table.begin(), code_table.find(index_buffer))); // Output code for index buffer
        code_stream.push_back(std::distance(code_table.begin(), code_table.find("EOI"))); // Output EOI code

        stream.write(reinterpret_cast<const char*>(&code_stream), code_stream.size());
    }

    stream.write(reinterpret_cast<const char*>(&trailer), sizeof(trailer));

    return *this;
}

Image& Image::crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) {
    if (x0 + width > get_width() || y0 + height > get_height())
        throw;

    std::vector<uint8_t> new_pixel_data;

    for (uint32_t y = y0; y < y0 + height; ++y) {
        for (uint32_t x = x0; x < x0 + width; ++x) {
            Color current_pixel = get_pixel(x, y);
            // TODO: change with channel amount
            new_pixel_data.push_back(current_pixel.B);
            new_pixel_data.push_back(current_pixel.G);
            new_pixel_data.push_back(current_pixel.R);
        }
    }

    set_dimensions(width, height);
    set_pixel_data(new_pixel_data);

    return *this;
}

Image& Image::flip() {
    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(get_pixel_data().size());

    uint8_t channels = get_channels();
    std::vector<uint8_t> &pixel_data = get_pixel_data();

    for (uint32_t y = 0; y < get_height(); ++y) {
        for (uint32_t x = 0; x < get_width(); ++x) {
            uint32_t source_pixel = channels * (y * get_width() + x);
            uint32_t destination_pixel = channels * ((get_height() - y - 1) * get_width() + x);

            // TODO: change with channel amount
            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];
        }
    }

    set_pixel_data(new_pixel_data);

    return *this;
}

Image& Image::rotate_clockwise() {
    uint8_t channels = get_channels();
    std::vector<uint8_t> &pixel_data = get_pixel_data();

    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(pixel_data.size());

    for (uint32_t y = 0; y < get_height(); ++y) {
        for (uint32_t x = 0; x < get_width(); ++x) {
            uint32_t source_pixel = channels * (y * get_width() + x);
            uint32_t destination_pixel = channels * ((get_height() - y - 1) + get_height() * x);

            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];

            if (channels == 4)
                new_pixel_data[destination_pixel + 3] = pixel_data[source_pixel + 3];
        }
    }

    set_pixel_data(new_pixel_data);
    set_dimensions(get_height(), get_width());

    return *this;
}

Image& Image::rotate_counterclockwise() {
    uint8_t channels = get_channels();
    std::vector<uint8_t> &pixel_data = get_pixel_data();

    std::vector<uint8_t> new_pixel_data;
    new_pixel_data.resize(pixel_data.size());

    for (uint32_t y = 0; y < get_height(); ++y) {
        for (uint32_t x = 0; x < get_width(); ++x) {
            uint32_t source_pixel = channels * (y * get_width() + x);
            uint32_t destination_pixel = channels * (y + get_height() * x);

            new_pixel_data[destination_pixel + 0] = pixel_data[source_pixel + 0];
            new_pixel_data[destination_pixel + 1] = pixel_data[source_pixel + 1];
            new_pixel_data[destination_pixel + 2] = pixel_data[source_pixel + 2];

            if (channels == 4)
                new_pixel_data[destination_pixel + 3] = pixel_data[source_pixel + 3];
        }
    }

    set_pixel_data(new_pixel_data);
    set_dimensions(get_height(), get_width());

    return *this;
}

Image& Image::grayscale() {
    for (uint32_t y = 0; y < get_height(); ++y) {
        for (uint32_t x = 0; x < get_width(); ++x) {
            Color color = get_pixel(x, y);
            uint32_t grayscale = color.R * .299 + color.G * .587 + color.B * .144;
            set_pixel(x, y, Color(grayscale, grayscale, grayscale));
        }
    }

    return *this;
}

Image& Image::inverse() {
    for (uint32_t y = 0; y < get_height(); ++y) {
        for (uint32_t x = 0; x < get_width(); ++x) {
            Color color = get_pixel(x, y);
            set_pixel(x, y, Color(0xFF - color.R, 0xFF - color.G, 0xFF - color.B));
        }
    }

    return *this;
}

double cubic_interpolate (double Q[4], double x) {
    //double A = (Q[3] - Q[2]) - (Q[0] - Q[1]);
    //double B = (Q[0] - Q[1]) - A;
    //double C = Q[2] - Q[0];
    //double D = Q[1];
    //return D + x * (C + x * (B + x * A));
    return Q[1] + .5 * x * (Q[2] - Q[0] + x * (2 * Q[0] - 5 * Q[1] + 4 * Q[2] - Q[3] + x * (3 * (Q[1] - Q[2]) + Q[3]- Q[0])));
}

double bicubic_interpolate (double Q[4][4], uint32_t x, uint32_t y) {
    double temp[4];
    temp[0] = cubic_interpolate(Q[0], y);
    temp[1] = cubic_interpolate(Q[1], y);
    temp[2] = cubic_interpolate(Q[2], y);
    temp[3] = cubic_interpolate(Q[3], y);
    return cubic_interpolate(temp, x);
}

Image& Image::resize(uint32_t width, uint32_t height, Interpolation mode) {
    std::vector<uint8_t> new_pixel_data;

    float x_ratio, y_ratio;
    switch (mode) {
        case Interpolation::NearestNeighbour:
            x_ratio = static_cast<float>(width) / static_cast<float>(get_width());
            y_ratio = static_cast<float>(height) / static_cast<float>(get_height());
            break;
        case Interpolation::Bilinear:
            x_ratio = static_cast<float>(get_width() - 1) / static_cast<float>(width); // NOTE: Using these ratios for NN interpolation gives an interesting result
            y_ratio = static_cast<float>(get_height() - 1) / static_cast<float>(height);
            break;
        case Interpolation::Bicubic:
            x_ratio = static_cast<float>(get_width()) / static_cast<float>(width);
            y_ratio = static_cast<float>(get_height()) / static_cast<float>(height);
            break;
    }

    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            switch (mode) {
                case Interpolation::NearestNeighbour: {
                    uint32_t source_x = std::round(x / x_ratio);
                    uint32_t source_y = std::round(y / y_ratio);

                    Color source_pixel = get_pixel(source_x, source_y);

                    new_pixel_data.push_back(source_pixel.B);
                    new_pixel_data.push_back(source_pixel.G);
                    new_pixel_data.push_back(source_pixel.R);

                    break;
                }

                case Interpolation::Bilinear: {
                    uint32_t source_x = x_ratio * x;
                    uint32_t source_y = y_ratio * y;

                    float x_diff = (x_ratio * x) - source_x;
                    float y_diff = (y_ratio * y) - source_y;

                    Color Q11 = get_pixel(source_x, source_y);
                    Color Q21 = get_pixel(source_x + 1, source_y);
                    Color Q12 = get_pixel(source_x, source_y + 1);
                    Color Q22 = get_pixel(source_x + 1, source_y + 1);

                    float blue = Q11.B * (1 - x_diff) * (1 - y_diff) + Q21.B * x_diff * (1 - y_diff) + Q12.B * y_diff * (1 - x_diff) + Q22.B * (x_diff * y_diff);
                    float green = Q11.G * (1 - x_diff) * (1 - y_diff) + Q21.G * x_diff * (1 - y_diff) + Q12.G * y_diff * (1 - x_diff) + Q22.G * (x_diff * y_diff);
                    float red = Q11.R * (1 - x_diff) * (1 - y_diff) + Q21.R * x_diff * (1 - y_diff) + Q12.R * y_diff * (1 - x_diff) + Q22.R * (x_diff * y_diff);

                    new_pixel_data.push_back(static_cast<uint8_t>(blue));
                    new_pixel_data.push_back(static_cast<uint8_t>(green));
                    new_pixel_data.push_back(static_cast<uint8_t>(red));
                    break;
                }

                case Interpolation::Bicubic: {
                    float x_origin = x_ratio * x;
                    float y_origin = y_ratio * y;

                    float x_origin_floor = std::floor(x_origin);
                    float y_origin_floor = std::floor(y_origin);

                    float x_origin_frac = x_origin - x_origin_floor;
                    float y_origin_frac = y_origin - y_origin_floor;

                    double Q[4][4] {0};
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            int t_x = std::clamp(static_cast<int>(std::round(x_origin_floor)) + j - 1, 0, static_cast<int>(get_width()) - 1);
                            int t_y = std::clamp(static_cast<int>(std::round(y_origin_floor)) + i - 1, 0, static_cast<int>(get_height()) - 1);
                            Q[i][j] = get_pixel(t_x, t_y).B;
                        }
                    }

                    new_pixel_data.push_back(static_cast<uint8_t>(std::round(bicubic_interpolate(Q, x_origin_frac, y_origin_frac))));
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            int t_x = std::clamp(static_cast<int>(std::round(x_origin_floor)) + j - 1, 0, static_cast<int>(get_width()) - 1);
                            int t_y = std::clamp(static_cast<int>(std::round(y_origin_floor)) + i - 1, 0, static_cast<int>(get_height()) - 1);
                            Q[i][j] = get_pixel(t_x, t_y).G;
                        }
                    }

                    new_pixel_data.push_back(static_cast<uint8_t>(std::round(bicubic_interpolate(Q, x_origin_frac, y_origin_frac))));
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            int t_x = std::clamp(static_cast<int>(std::round(x_origin_floor)) + j - 1, 0, static_cast<int>(get_width()) - 1);
                            int t_y = std::clamp(static_cast<int>(std::round(y_origin_floor)) + i - 1, 0, static_cast<int>(get_height()) - 1);
                            Q[i][j] = get_pixel(t_x, t_y).R;
                        }
                    }

                    new_pixel_data.push_back(static_cast<uint8_t>(std::round(bicubic_interpolate(Q, x_origin_frac, y_origin_frac))));
                    break;
                }
            }
        }
    }

    set_pixel_data(new_pixel_data);
    set_dimensions(width, height);

    return *this;
}

uint32_t Color::raw() {
    return static_cast<uint32_t>(R) << 16 | (static_cast<uint32_t>(G) << 8 | static_cast<uint32_t>(B));
}
