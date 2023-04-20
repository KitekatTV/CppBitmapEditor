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
