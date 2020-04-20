PROJECT_NAME     := EnoceanSwitch
TARGETS          := receiver sender
OUTPUT_DIRECTORY := build
TEMPLATE_PATH    := system
#VERBOSE          := 1


# Include folders
INC_FOLDERS += system

# Source files
SRC_FILES += system/gcc_startup_nrf52840.S
SRC_FILES += system/system_nrf52840.c

# Target specific source files
SRC_FILES_receiver += receiver.c
SRC_FILES_sender += sender.c

# Libraries
LIB_FILES +=
LIB_FILES += -lc -lnosys -lm -lstdc++ # add standard libs at end so that their symbols get found

# Linker script
LINKER_SCRIPT    := link.ld

# C flags common to all targets
#CFLAGS += -Og -g3 # debug
CFLAGS += -O3 -g3 # release
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums
CFLAGS += -D__HEAP_SIZE=8192
CFLAGS += -D__STACK_SIZE=8192
CFLAGS += -Wall

# C++ flags (C++ also uses CFLAGS)
CXXFLAGS +=

# Assembler flags
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -D__HEAP_SIZE=8192
ASMFLAGS += -D__STACK_SIZE=8192

# Linker flags
#LDFLAGS += -flto # link time optimization
LDFLAGS += -mthumb -mabi=aapcs -L$(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -Wl,--gc-sections # let linker dump unused sections
LDFLAGS += --specs=nano.specs # use newlib in nano version



.PHONY: default help

# Default target - first one defined
default: receiver sender

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		receiver


include system/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))
