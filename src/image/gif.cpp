#include "gif.h"

GIF& GIF::read(std::string file_name) {
    /* std::ifstream stream(file_name, std::ios::binary);

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

    return *this;*/

    throw "Under construction";
}

std::vector<uint8_t> GIF::GifImage::full_pixel_data() {
    std::vector<uint8_t> data = pixel_data[0].data;

    for (int i = 1; i < pixel_data.size(); ++i)
        data.insert(data.end(), pixel_data[i].data.begin(), pixel_data[i].data.end());

    return data;
}

GIF& GIF::write(std::string file_name) {
    /*
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

    return *this;*/

    throw "Under construction";
}