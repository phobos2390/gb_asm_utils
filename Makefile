NAME := utils
TEST_NAME := test_$(NAME)
MBC_TYPE := 0x1B
RAM_SIZE := 0x2
TEST_ENGINE_DIR := ./gb_asm_test/src
TEST_DIRECTORY := ./src/test
ADDITIONAL_INCLUDES := "-i ./src"

build_test:
	make -f gb_asm_test/Makefile build_test \
	    ADDITIONAL_INCLUDES=$(ADDITIONAL_INCLUDES) \
	    TEST_ENGINE_DIR=$(TEST_ENGINE_DIR) \
	    TEST_DIRECTORY=$(TEST_DIRECTORY) \
	    TEST_NAME=$(TEST_NAME)

clean:
	rm -rf build/

test: build_test
	sameboy_debugger build/$(TEST_NAME).gb

all: build_test

