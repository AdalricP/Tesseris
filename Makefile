# Simple root-level Makefile to build from repository root

CC := clang
INCLUDES := -I/opt/homebrew/include/SDL2 -I/opt/homebrew/include
CFLAGS := -g -fcolor-diagnostics -fansi-escape-codes $(INCLUDES)
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lSDL2 -lvulkan

SRC_DIR := src
BUILD_DIR := $(SRC_DIR)/build/Debug

SRCS := \
  $(SRC_DIR)/main.c \
  $(SRC_DIR)/application.c \
  $(SRC_DIR)/sdl_window.c \
  $(SRC_DIR)/vulkan/vulkan_instance.c \
  $(SRC_DIR)/vulkan/vulkan_surface.c \
  $(SRC_DIR)/vulkan/vulkan_physical_device.c \
  $(SRC_DIR)/vulkan/vulkan_logical_device.c \
  $(SRC_DIR)/swapchain/swapchain.c \
  $(SRC_DIR)/renderpass/renderpass.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET := $(BUILD_DIR)/outDebug

.PHONY: all run clean dirs

all: $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/swapchain/ \
  @mkdir -p $(BUILD_DIR)/vulkan/ \
  @mkdir -p $(BUILD_DIR)/renderpass/ \
  
  

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf $(BUILD_DIR)


