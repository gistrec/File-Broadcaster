program:
	g++ src/Main.cpp      \
	-std=c++14 -pthread   \
	-Ilib/cxxopts/include \
	-o FileBroadcaster

gtests:
	g++ tests/Tests.cpp   \
	-std=c++14 -pthread   \
	-Ilib/cxxopts/include \
	-lgtest               \
	-o GTests
