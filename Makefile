ARMGNU ?= aarch64-linux-gnu

PI_VERSION ?= 4
COPS = -DRPI_VERSION=$(PI_VERSION) -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude

BUILD_DIR = build
SRC_DIR = src

ARMSTUB_BUILD_DIR = armstub/build
ARMSTUB_SRC_DIR = armstub/src

all : kernel8.img armstub

kernel: kernel8.img

clean :
	rm -rf $(BUILD_DIR) $(ARMSTUB_BUILD_DIR) *.d *.img *.bin

screen :
	screen /dev/ttyUSB0 115200

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: ./linker.ld $(OBJ_FILES)
	@echo "Building for PI version = $(value PI_VERSION)"
	$(ARMGNU)-ld -T ./linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

$(ARMSTUB_BUILD_DIR)/%_s.o: $(ARMSTUB_SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@

armstub: $(ARMSTUB_BUILD_DIR)/armstub_s.o
	@echo "Building armstub-new.bin"
	$(ARMGNU)-ld --section-start=.text=0 -o $(ARMSTUB_BUILD_DIR)/armstub.elf $(ARMSTUB_BUILD_DIR)/armstub_s.o
	$(ARMGNU)-objcopy $(ARMSTUB_BUILD_DIR)/armstub.elf -O binary armstub-new.bin