ABSPATH = $(PWD)
PROJECT_PATH = $(ABSPATH)/src
TESTS_PATH = $(ABSPATH)/tests
BUILD_PATH = $(ABSPATH)/build

CC := clang
HDRS :=
CFLAGS := `/usr/local/bin/sdl2-config --static-libs --cflags` \
	-lSDL2 \
	-lSDL2_image \
	-lm \
	-Wall -ggdb3 -O0 --std=c99

all: run

clean:
	rm -rf $(BUILD_PATH)

prepare_dirs:
	mkdir -p $(BUILD_PATH)/bin \
		$(BUILD_PATH)/src
	cp -r $(PROJECT_PATH) $(BUILD_PATH)

# test: libs
# 	cp $(BUILD_PATH)/src/*.h $(BUILD_PATH)/lib/headers
# 	$(GCC_BINARY) -Wall $(TESTS_PATH)/*.c \
# 		-I$(BUILD_PATH)/lib/headers \
# 		-L$(BUILD_PATH)/lib \
# 		-l_AStarHelpers \
# 		-l_AStarClasses \
# 		-o $(BUILD_PATH)/bin/test
# 	$(BUILD_PATH)/bin/test

# libs: clean prepare_dirs
# 	# STATIC BUILD
# 	cd $(BUILD_PATH)/src && \
# 		$(GCC_BINARY) -c $(BUILD_PATH)/src/helpers.c -o $(BUILD_PATH)/lib/helpers.o && \
# 		$(GCC_BINARY) -c $(BUILD_PATH)/src/classes.c -o $(BUILD_PATH)/lib/classes.o
#
# 	cd $(BUILD_PATH)/lib && \
# 		ar rcs lib_AStarHelpers.a helpers.o && \
# 		ar rcs lib_AStarClasses.a classes.o

run: clean prepare_dirs
	${CC} ${CFLAGS} -o $(BUILD_PATH)/bin/project \
		$(BUILD_PATH)/src/classes/*.c \
		$(BUILD_PATH)/src/*.c
	$(BUILD_PATH)/bin/project


.PHONY: all clean
