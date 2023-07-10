MAKEFILE_DIR=$(dir $(realpath $(firstword $(MAKEFILE_LIST))))
PROJECT_DIR=$(realpath $(MAKEFILE_DIR)..)
TEST_DIR=$(PROJECT_DIR)/Test

CPPUTEST_HOME=$(TEST_DIR)/cpputest
COMPONENT_NAME=tests
CPPUTEST_OBJS_DIR=build
CPPUTEST_LIB_DIR=build
# project source files path
SRC_DIRS=$(PROJECT_DIR)

# project header files path
INCLUDE_DIRS=$(PROJECT_DIR)

# add this to avoid problems when including cpputest headers on test files
INCLUDE_DIRS+=$(CPPUTEST_HOME)/include

## specify where the test code is located
TEST_SRC_DIRS=$(TEST_DIR)
TEST_SRC_FILES= \
$(PROJECT_DIR)/uprint.c

## what to call the test binary
TEST_TARGET=test_runner

## add custom flags
CPPUTEST_CFLAGS+=-DUSING_FLOAT=1 -DUSING_LONG_LONG=1

#run MakefileWorker.mk with the variables defined here
include $(CPPUTEST_HOME)/build/MakefileWorker.mk