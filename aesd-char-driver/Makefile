<<<<<<< HEAD

UNITY_ROOT=test/Unity/

# CFLAGS=-std=c89
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition



default: all

UNAME_P := $(shell uname -p)

TARGET_SRC = todoobj
TARGET_TEST = aesd-cirular-buffer_test

SRC_FILES=src/aesd-circular-buffer.c
TEST_SRC_FILES=$(UNITY_ROOT)/src/unity.c src/aesd-circular-buffer.c test/aesd-circular-buffer_test.c test/test_runners/aesd-circular-buffer_test_runner.c
INC_DIRS=-Isrc -I$(UNITY_ROOT)/src
SYMBOLS=

clean:
	rm -f todoobj
	rm -f aesd-circular-buffer_test

all: clean $(TARGET_SRC)

$(TARGET_SRC):
	$(CC) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET_SRC)

test: test_runner
	$(CC) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) -g $(TEST_SRC_FILES) -o $(TARGET_TEST)

test_runner: test/aesd-circular-buffer_test.c
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test/aesd-circular-buffer_test.c test/test_runners/aesd-circular-buffer_test_runner.c





=======
# See example Makefile from scull project
# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
  # call from kernel build system
  obj-m	:= aesdchar.o
  aesdchar-y := aesd-circular-buffer.o main.o
else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
#>>>>>>> assignments-base/assignment8

