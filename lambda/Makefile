SRC_DIR:=.
BUILD_DIR:=../../build

ARCH!=../conf/uname.sh

ifeq ($(ARCH),x86_64)
    CC:= g++
    CFLAGS:= -m64
else ifeq ($(ARCH),arm)
    CC:= arm-gcc
    CFLAGS:= -marm
else
    $(error Unknown architecture)
endif


# 二级优化
CFLAGS+= -O2

# CFLAGS+= -std=c++14

# 添加可调试选项
#CFLAGS+= -g

CFLAGS:=$(strip ${CFLAGS})

include ./make.conf
include ./stdlib/make.conf


OBJS:=\
$(BUILD_DIR)/lambda.o \
$(INTERPRETER_OBJS) \
$(STDLIB_LIBA) \

$(BUILD_DIR)/lambda.o: $(SRC_DIR)/lambda.cpp
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@


INTERPRETER:=$(BUILD_DIR)/lambda

$(INTERPRETER): $(OBJS)
	@echo linking...
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	@echo make complete!

.PHONY: target
target:
	@echo $(OBJS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: install
install: $(INTERPRETER)

