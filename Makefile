# Compiler and Flags
CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -MMD -Iinclude
AR      := ar
ARFLAGS := rcs

# Target Library Name
TARGET_DIR := lib
TARGET  := $(TARGET_DIR)/libecassert.a

# Directories
SRCDIR  := src
OBJDIR  := obj

# Sources and Objects
SRCS    := $(wildcard $(SRCDIR)/*.c)
OBJS    := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPS    := $(OBJS:.o=.d)

# Default Rule
all: $(TARGET)

# Link the Static Library
$(TARGET): $(OBJS)
	@echo Making TARGET
	mkdir -p $(TARGET_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Compile Source Files to Object Files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo Making OBJDIR - $<
	$(CC) $(CFLAGS) -c $< -o $@

# Create Object Directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Include automatically generated dependency files (.d)
-include $(DEPS)

# Clean Build Files
.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) $(TARGET_DIR)
