CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -lncurses -fsanitize=address \
	-Wno-error=unused-variable -Wno-error=unused-parameter -Wno-error=parentheses

BUILD = build
OUTPUT = $(BUILD)/cm2-riscv-emulator
OUTPUT_ARGS = emulator-bin/cmes.bin

CSRCS = $(shell find src -name '*.c')
OBJS = $(patsubst %.c,$(BUILD)/%.o,$(CSRCS))

all: clean compile run

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