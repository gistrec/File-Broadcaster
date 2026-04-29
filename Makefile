program:
	g++ src/Main.cpp src/Receiver.cpp src/Sender.cpp src/Config.cpp \
	-std=c++14 -pthread   \
	-Ilib/cxxopts/include \
	-o FileBroadcaster

gtests:
	g++ tests/Tests.cpp   \
	-std=c++14 -pthread   \
	-Ilib/cxxopts/include \
	-lgtest               \
	-o GTests
