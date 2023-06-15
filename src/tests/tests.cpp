#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "image.h"
#include "bitmap.h"
#include "gif.h"

TEST(Bitmap, Read) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.get_pixel_data();
	std::vector<uint8_t> test_data {
		255, 255, 255,  0, 0, 0,  0, 0, 0, 255, 255, 255,
		0, 0, 0,  255, 255, 255,  255, 255, 255,  0, 0, 0,
		255, 255, 255,  0, 0, 0,  0, 0, 0, 255, 255, 255,
		0, 0, 255,  0, 255, 0,  255, 0, 0,  0, 0, 0,
	};

	ASSERT_EQ(data, test_data);
}

TEST(Bitmap, RotateClockwise) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.rotate_clockwise().get_pixel_data();
	std::vector<uint8_t> test_data {
		0, 0, 255,  255, 255, 255,  0, 0, 0,  255, 255, 255,
		0, 255, 0,  0, 0, 0,  255, 255, 255,  0, 0, 0,
		255, 0, 0,  0, 0, 0,  255, 255, 255,  0, 0, 0,
		0, 0, 0,  255, 255, 255,  0, 0, 0,  255, 255, 255,
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, RotateCounterClockwise) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.rotate_counterclockwise().get_pixel_data();
	std::vector<uint8_t> test_data {
		255, 255, 255,  0, 0, 0,  255, 255, 255,  0, 0, 255,
		0, 0, 0,  255, 255, 255,  0, 0, 0,  0, 255, 0,
		0, 0, 0,  255, 255, 255,  0, 0, 0,  255, 0, 0,
		255, 255, 255,  0, 0, 0,  255, 255, 255,  0, 0, 0,  
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, Flip) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.flip().get_pixel_data();
	std::vector<uint8_t> test_data {
		0, 0, 255,  0, 255, 0,  255, 0, 0,  0, 0, 0,
		255, 255, 255,  0, 0, 0,  0, 0, 0, 255, 255, 255,
		0, 0, 0,  255, 255, 255,  255, 255, 255,  0, 0, 0,
		255, 255, 255,  0, 0, 0,  0, 0, 0, 255, 255, 255,
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, Grayscale) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.grayscale().get_pixel_data();
	std::vector<uint8_t> test_data {
		6, 6, 6,  0, 0, 0,  0, 0, 0, 6, 6, 6,
		0, 0, 0,  6, 6, 6,  6, 6, 6,  0, 0, 0,
		6, 6, 6,  0, 0, 0,  0, 0, 0, 6, 6, 6,
		76, 76, 76,  149, 149, 149,  36, 36, 36,  0, 0, 0,
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, Inverse) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.inverse().get_pixel_data();
	for (uint8_t i : data)
		std::cout << std::to_string(i);
	std::vector<uint8_t> test_data {
		0, 0, 0,  255, 255, 255,  255, 255, 255,  0, 0, 0,
		255, 255, 255,  0, 0, 0,  0, 0, 0, 255, 255, 255,
		0, 0, 0,  255, 255, 255,  255, 255, 255,  0, 0, 0,
		255, 255, 0,  255, 0, 255,  0, 255, 255,  255, 255, 255,
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, Crop) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	std::vector<uint8_t> data = bmp.crop(0, 0, 2, 2).get_pixel_data();
	std::vector<uint8_t> test_data {
		255, 255, 255,  0, 0, 0,
		0, 0, 0,  255, 255, 255,
	};

	EXPECT_EQ(data, test_data);
}

TEST(Bitmap, Write) {
	Bitmap bmp("../src/tests/test_bmp.bmp");
	bmp.write("../src/tests/test_bmp_out.bmp");
	Bitmap bmp_new("../src/tests/test_bmp_out.bmp");

	EXPECT_EQ(bmp.get_pixel_data(), bmp_new.get_pixel_data());
	EXPECT_EQ(bmp.get_channels(), bmp_new.get_channels());
	EXPECT_EQ(bmp.get_width(), bmp_new.get_width());
	EXPECT_EQ(bmp.get_height(), bmp_new.get_height());
}
