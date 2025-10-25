# Simple root-level Makefile to build from repository root

CC := clang
GLSLC := /opt/homebrew/opt/glslang/bin/glslang
INCLUDES := -I/opt/homebrew/include/SDL2 -I/opt/homebrew/include
CFLAGS := -g -fcolor-diagnostics -fansi-escape-codes $(INCLUDES)
LDFLAGS := -L/opt/homebrew/lib
LIBS := -lSDL2 -lvulkan

SRC_DIR := src
BUILD_DIR := $(SRC_DIR)/build/Debug
SHADER_DIR := shaders

# Shader files
SHADERS := $(wildcard $(SHADER_DIR)/*.vert $(SHADER_DIR)/*.frag)
SHADER_SPVS := $(SHADERS:%=%.spv)

SRCS := \
  $(SRC_DIR)/main.c \
  $(SRC_DIR)/application.c \
  $(SRC_DIR)/sdl_window.c \
  $(SRC_DIR)/vulkan/vulkan_instance.c \
  $(SRC_DIR)/vulkan/vulkan_surface.c \
  $(SRC_DIR)/vulkan/vulkan_physical_device.c \
  $(SRC_DIR)/vulkan/vulkan_logical_device.c \
  $(SRC_DIR)/vulkan/vulkan_depth.c \
  $(SRC_DIR)/swapchain/swapchain.c \
  $(SRC_DIR)/renderpass/renderpass.c \
  $(SRC_DIR)/renderpass/framebuffer/framebuffer.c \
  $(SRC_DIR)/renderpass/commandbuffers/commandbuffers.c \
  $(SRC_DIR)/graphics_pipeline/pipeline_layout.c \
  $(SRC_DIR)/graphics_pipeline/shader_module.c \
  $(SRC_DIR)/graphics_pipeline/graphics_pipeline.c \
  $(SRC_DIR)/graphics_pipeline/buffer.c \
  $(SRC_DIR)/sync/synchronization.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET := $(BUILD_DIR)/outDebug

.PHONY: all run clean dirs play shaders

all: $(TARGET)

shaders: $(SHADER_SPVS)
	@echo "Shaders compiled"

play:
	@clear
	@$(MAKE) clean
	@$(MAKE) all
	@echo "Checking for glslang..."
	@test -f $(GLSLC) && $(MAKE) shaders || echo "Note: glslang not found, skipping shader compilation"
	@$(MAKE) run

%.vert.spv: %.vert
	@echo "Compiling vertex shader: $<"
	@$(GLSLC) -V $< -o $@

%.frag.spv: %.frag
	@echo "Compiling fragment shader: $<"
	@$(GLSLC) -V $< -o $@

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/swapchain
	@mkdir -p $(BUILD_DIR)/vulkan
	@mkdir -p $(BUILD_DIR)/renderpass
	@mkdir -p $(BUILD_DIR)/renderpass/framebuffer
	@mkdir -p $(BUILD_DIR)/renderpass/commandbuffers
	@mkdir -p $(BUILD_DIR)/graphics_pipeline
	@mkdir -p $(BUILD_DIR)/sync
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

run: $(TARGET)
	$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SHADER_SPVS)



