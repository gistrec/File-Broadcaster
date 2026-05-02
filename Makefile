CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pthread
INCLUDES  = -Ilib/cxxopts/include
LDLIBS_WIN ?=

SRCS    = src/Main.cpp src/Receiver.cpp src/Sender.cpp src/Config.cpp
TARGET  = FileBroadcaster

GTEST_CFLAGS  ?=
GTEST_LDFLAGS ?=

.PHONY: all program gtests e2e clean

all: program

program: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(LDLIBS_WIN)

gtests:
	$(CXX) tests/Tests.cpp \
		$(CXXFLAGS) \
		$(INCLUDES) \
		$(GTEST_CFLAGS) \
		-lgtest \
		$(GTEST_LDFLAGS) \
		-o GTests

e2e: program
	BINARY=./$(TARGET) bash tests/e2e.sh

clean:
	rm -f $(TARGET) $(TARGET).exe GTests GTests.exe
