#include "image.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <type_traits>

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

    float x_ratio{0}, y_ratio{0};
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
