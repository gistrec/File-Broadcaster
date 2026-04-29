program:
	g++ src/Main.cpp src/Receiver.cpp src/Sender.cpp src/Config.cpp \
	-std=c++14 -pthread   \
	-Ilib/cxxopts/include \
	-o FileBroadcaster

GTEST_CFLAGS  ?=
GTEST_LDFLAGS ?=

gtests:
	g++ tests/Tests.cpp   \
	-std=c++17 -pthread   \
	-Ilib/cxxopts/include \
	$(GTEST_CFLAGS)       \
	-lgtest               \
	$(GTEST_LDFLAGS)      \
	-o GTests
