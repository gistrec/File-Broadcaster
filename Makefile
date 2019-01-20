all:
	g++ src/Main.cpp      \
	-std=c++14 -pthread   \
	-o FileBroadcaster
