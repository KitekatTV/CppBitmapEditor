C++ Image Editor (imeg)
----------------
imed (short for IMage EDitor) is a command line oriented tool to perform basic transformations to GIF and Bitmap files.

### Installation

While imed is not available as prebuilt binary, you can compile it yourself.

* For those, who are familiar with nix, it can be used to fetch all dependencies and build or install automatically (Make sure to enable flakes):

```
$ nix build
```

* Otherwise, use CMake:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Usage

imed can be used to perform variety of simple transformations, such as:

| Command | Description | Example |
|---------|-------------|---------|
| crop | crops the image, leaving zone with given dimensions from given point | imed crop image.bmp 10 10 200 200 |
| flip | flips the image | imed flip image.bmp |
| rcw | rotates the image clock-wise | imed rcw image.bmp | 
| rcw | rotates the image counter clock-wise | imed rccw image.bmp | 
| grayscale | applies grayscale to the image. does not change bit depth | imed grayscale image.bmp | 
| inverse | applies inverse to the image | imed grayscale image.bmp | 
| resize | resizes the image using given interpolation type | imed resize image.bmp 1280 920 bil | 
