# Build for the GGA Archipelago mod.
#
# Mirrors MMRecompRando's Makefile, minus the decomp include paths: GGA has no
# decomp, so the mod compiles against its own headers only.

BUILD_DIR   := build
OUTPUT_NAME := gga_recomp_rando
MOD_TOML    := mod.toml

MOD_TOOL := ../N64RecompSource/cmake-build-modtool/Release/RecompModTool.exe

TARGET   := $(BUILD_DIR)/mod.elf
LDSCRIPT := mod.ld

CC ?= clang
LD ?= ld.lld

CFLAGS   := -target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg \
            -mno-check-zero-division -fomit-frame-pointer -ffast-math \
            -fno-unsafe-math-optimizations -fno-builtin-memset \
            -Wall -Wextra -Wno-incompatible-library-redeclaration \
            -Wno-unused-parameter -Wno-unknown-pragmas -Wno-unused-variable \
            -Wno-missing-braces -Wno-unsupported-floating-point-opt -Werror=section

CPPFLAGS := -nostdinc -D_LANGUAGE_C -DMIPS -I include
LDFLAGS  := -nostdlib -T $(LDSCRIPT) -Map $(BUILD_DIR)/mod.map \
            --unresolved-symbols=ignore-all --emit-relocs -e 0 --no-nmagic

C_SRCS := $(wildcard src/*.c)
C_OBJS := $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))
C_DEPS := $(C_OBJS:.o=.d)

all: $(OUTPUT_NAME)/mod_binary.bin

# Native libraries are picked up from lib/ by the mod tool.
LIBFILES := $(wildcard lib/*.dll)

$(OUTPUT_NAME)/mod_binary.bin: $(TARGET) $(MOD_TOML) $(LIBFILES)
	@mkdir -p $(OUTPUT_NAME)
	$(MOD_TOOL) $(MOD_TOML) $(OUTPUT_NAME)

$(TARGET): $(C_OBJS) $(LDSCRIPT)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(C_OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -MMD -MF $(@:.o=.d) -c -o $@

clean:
	rm -rf $(BUILD_DIR) $(OUTPUT_NAME)

-include $(C_DEPS)

.PHONY: all clean
