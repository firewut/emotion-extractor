ABSPATH = $(PWD)
PROJECT_PATH = $(ABSPATH)/src
TESTS_PATH = $(ABSPATH)/tests
BUILD_PATH = $(ABSPATH)/build
TEST_FILE_NAME ?= "/tmp/file_B2uYGL.mp4"

CC := g++
HDRS :=
CFLAGS := `pkg-config --cflags --libs opencv` \
	-lm \
	-Wall -std=c++11 -ggdb3 -O0

all: run

clean:
	rm -rf $(BUILD_PATH)

prepare_dirs:
	mkdir -p $(BUILD_PATH)/bin \
		$(BUILD_PATH)/src
	cp -r $(PROJECT_PATH) $(BUILD_PATH)

build: clean prepare_dirs
	${CC} ${CFLAGS} -o $(BUILD_PATH)/bin/project \
		$(BUILD_PATH)/src/*.cpp

run: build
	time $(BUILD_PATH)/bin/project $(TEST_FILE_NAME)

.PHONY: all clean
