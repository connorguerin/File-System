wadfs:
	g++ -c wad.cpp
	g++ -c tree.cpp
	ar cr libwad.a wad.o tree.o
	g++ -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 wadfs.cpp tree.cpp wad.cpp -o wadfs -lfuse -L -lwad
