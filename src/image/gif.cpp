#include "gif.h"
#include <bitset>
#include <cmath>
#include <iostream>

template<typename T>
int get_vector_index(std::vector<T> & vec, T element) {
	return std::distance(vec.begin(), std::find(vec.begin(), vec.end(), element));
}

std::vector<int> split_and_parse(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<int> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(std::stoi(token));
	}

	return res;
}

std::vector<std::string> GIF::generate_code_table() {
    std::vector<std::string> res;

    for (int i = 0; i < global_color_table.size(); ++i) {
        res.push_back(std::to_string(i));
    }

    res.push_back("CC");
    res.push_back("EOIC");

    return res;
}

std::vector<int> GIF::generate_code_stream() {
    std::vector<int> res;
    for (int i = 0; i < get_width(); ++i) {
        for (int j = 0; j < get_height(); ++i) {
            res.push_back(get_vector_index(global_color_table, get_pixel(i, j)));
        }
    }

    return res;
}

void write_data(GIF* gif, std::ofstream * stream, std::vector<int> index_stream, int min_code_size) {
	//std::vector<int> code_stream;
	std::string index_buffer = "";

	std::vector<bool> res; //!WRITER

	// Initialize code table (from GCT)
	std::vector<std::string> code_table = gif->generate_code_table(); //{ "0", "1", "2", "3", "CC", "EOIC" };

	// Send clear code to the code stream
	//code_stream.push_back(get_vector_index(code_table, std::string("CC")));
	//!WRITER
	std::string bits = std::bitset<32>(get_vector_index(code_table, std::string("CC"))).to_string();
	bits.erase(0, std::min(bits.find_first_not_of('0'), bits.size() - 1));

	std::vector<bool> temp;

	for (int i = bits.length() - 1; i >= 0; --i)
		temp.push_back(bits[i] == '1' ? true : false);

	if (temp.size() < min_code_size) {
		for (int i = 0; i < min_code_size - bits.length(); ++i)
			temp.push_back(false);
	}

	for (bool bit : temp)
		res.push_back(bit);


	/*std::cout << "Bits: " << bits << " | Reverse: ";
	for (bool i : temp)
		std::cout << i;

	std::cout << " | Code table last: #" << code_table.size() - 1 << " | Min code size: " << min_code_size << " | Code: " << get_vector_index(code_table, std::string("CC")) << std::endl;*/


	int i = 0;
	// Read first stream index
	index_buffer = std::to_string(index_stream[i]);

	// LOOP:
	// Get next stream index, aka 'K'
	while (i + 1 < index_stream.size()) {
		i++; // HACK: Replace with 'for'?

		std::string K = std::to_string(index_stream[i]);

		// Is index buffer in our code table?
		if (std::find(code_table.begin(), code_table.end(), index_buffer + K) != code_table.end()) {
			index_buffer += K;
		}
		else {
			code_table.push_back(index_buffer + K);
			//code_stream.push_back(get_vector_index(code_table, index_buffer));

			//!WRITER
			std::string bits = std::bitset<32>(get_vector_index(code_table, index_buffer)).to_string();
			bits.erase(0, std::min(bits.find_first_not_of('0'), bits.size() - 1));

			std::vector<bool> temp;

			for (int i = bits.length() - 1; i >= 0; --i)
				temp.push_back(bits[i] == '1' ? true : false);

			if (temp.size() < min_code_size) {
				for (int i = 0; i < min_code_size - bits.length(); ++i)
					temp.push_back(false);
			}

			for (bool bit : temp)
				res.push_back(bit);


			/*std::cout << "Bits: " << bits << " | Reverse: ";
			for (bool i : temp)
				std::cout << i;

			std::cout << " | Code table last: #" << code_table.size() - 1 << " | Min code size: " << min_code_size << " | Code: " << get_vector_index(code_table, index_buffer) << std::endl;*/


			if (code_table.size() - 1 == std::pow(2, min_code_size) - 1)
				min_code_size++;

			index_buffer = K;
			K = "";
		}
	}

	/*
	for (int byte = 0; byte < res.size() / 8; ++byte) {
		for (int bit = 7; bit >= 0; --bit) {
			std::cout << (res[bit] == true ? 1 : 0);
		}
	}*/

    stream->write(reinterpret_cast<const char*>(reinterpret_cast<uint64_t* const*>(&res)), res.size()); //  HACK: data() seems to be private?
	// Send remaining buffer to the code stream
	//code_stream.push_back(get_vector_index(code_table, index_buffer));

	// Send end-of-info code to the code stream
	//code_stream.push_back(get_vector_index(code_table, std::string("EOIC")));
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

GIF& GIF::write(std::string file_name) {
    std::ofstream stream(file_name, std::ios::binary);

    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    stream.write(reinterpret_cast<const char*>(&lsd), sizeof(lsd));

    if (((lsd.packed_field >> 7) & 0x1) == 0x1)
        for (Color color : global_color_table)
            stream.write(reinterpret_cast<const char*>(color.raw()), sizeof(uint8_t) * 3);
    else
        throw "Local color tables are not supported yet!";

    // Global color table

    for (GifImage current_image : images) {
        stream.write(reinterpret_cast<const char*>(&current_image.image_descriptor), sizeof(current_image.image_descriptor));
        stream.write(reinterpret_cast<const char*>(&current_image.pixel_data.min_code_size), sizeof(uint8_t));
        stream.write(reinterpret_cast<const char*>(&current_image.pixel_data.length), sizeof(uint8_t));
        std::vector<int> index_stream = generate_code_stream();
        write_data(this, &stream, index_stream, current_image.pixel_data.min_code_size);
        stream.write(reinterpret_cast<const char*>(&current_image.pixel_data.trailer), sizeof(uint8_t));
    }

    stream.write(reinterpret_cast<const char*>(&trailer), sizeof(trailer));
}

uint8_t GIF::get_channels() { throw "Under construction"; }

uint32_t GIF::get_width() { throw "Under construction"; }

uint32_t GIF::get_height() { throw "Under construction"; }

uint32_t GIF::get_horizontal_resolution() { throw "Under construction"; }

uint32_t GIF::get_vertical_resolution() { throw "Under construction"; }

std::vector<uint8_t>& GIF::get_pixel_data() { return images[0].pixel_data.data; }

GIF& GIF::set_dimensions(uint32_t width, uint32_t height) {
    images[0].image_descriptor.image_width = width;
    images[0].image_descriptor.image_height = height;
    return *this;
}

GIF& GIF::set_pixel_data(std::vector<uint8_t> new_pixel_data) {
    images[0].pixel_data.data = std::move(new_pixel_data);
    return *this;
}

GIF& GIF::crop(uint32_t x0, uint32_t y0, uint32_t width, uint32_t height) {
    Image::crop(x0, y0, width, height);
    return *this;
};

GIF& GIF::flip() {
    Image::flip();
    return *this;
};

GIF& GIF::rotate_clockwise() {
    Image::rotate_clockwise();
    return *this;
};

GIF& GIF::rotate_counterclockwise() {
    Image::rotate_counterclockwise();
    return *this;
};

GIF& GIF::grayscale() {
    Image::grayscale();
    return *this;
};

GIF& GIF::inverse() {
    Image::inverse();
    return *this;
};

GIF& GIF::resize(uint32_t width, uint32_t height, Interpolation mode) {
    Image::resize(width, height, mode);
    return *this;
};

Color GIF::get_pixel(uint32_t x, uint32_t y) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t>& pixel_data = get_pixel_data();

    uint8_t B = pixel_data[current_pixel + 0];
    uint8_t G = pixel_data[current_pixel + 1];
    uint8_t R = pixel_data[current_pixel + 2];

    return Color(R, G, B);
};

GIF& GIF::set_pixel(uint32_t x, uint32_t y, Color color) {
    uint32_t current_pixel = get_channels() * (y * get_width() + x);
    std::vector<uint8_t>& pixel_data = get_pixel_data();

    pixel_data[current_pixel + 0] = color.B;
    pixel_data[current_pixel + 1] = color.G;
    pixel_data[current_pixel + 2] = color.R;

    return *this;
};
