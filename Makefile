CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -lncurses -fsanitize=address \
	-Wno-error=unused-variable -Wno-error=unused-parameter -Wno-error=parentheses

RAYLIB ?= true
OPTIMIZE ?= false

ifeq ($(RAYLIB), true)
	CFLAGS += -lraylib -DRAYLIB
endif

ifeq ($(OPTIMIZE), true)
	CFLAGS += -O3 -flto
endif

ROOT ?= .
BUILD = $(ROOT)/build
OUTPUT = $(BUILD)/cm2-riscv-emulator
OUTPUT_ARGS ?= emulator-bin/minesweeper.bin emulator-tilesheet/minesweeper.bmp

CSRCS = $(shell find $(ROOT)/src -name '*.c')
RELCSRCS = $(patsubst $(ROOT)/%,%,$(CSRCS))
OBJS = $(patsubst %.c,$(BUILD)/%.o,$(RELCSRCS))

all: clean compile

force:

compile: $(OBJS) $(OUTPUT)

$(BUILD)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(OUTPUT)

run:
	./$(OUTPUT) $(OUTPUT_ARGS)

clean:
	rm -rf $(BUILD)
