use path

mkdir Debug
cd Debug
make ..
cmake --build .

if (path:is-dir 'C:\') {
	mklink compile_commands.json ..
} else {
	ln -s compile_commands.json ..
}

cd ..
tar -xvf bitmaps.tar

cd bitmaps
mkdir test_640x426
mkdir test_1920x1280
mkdir test_5184x3456
