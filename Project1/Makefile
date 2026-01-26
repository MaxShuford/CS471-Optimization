CC=gcc
CFLAGS=-O2 -std=c11 -Wall -Wextra -pedantic -Iinclude
LDFLAGS=-lm

SRC_DIR=src
OBJ_DIR=build
INCLUDE_DIR=include

SRCS=$(SRC_DIR)/main.c \
     $(SRC_DIR)/mt19937ar.c \
     $(SRC_DIR)/config.c \
     $(SRC_DIR)/problem.c \
     $(SRC_DIR)/population.c \
     $(SRC_DIR)/csv.c \
     $(SRC_DIR)/timing.c

OBJS=$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ifeq ($(OS),Windows_NT)
TARGET=project1.exe
RM=del /Q
MKDIR=mkdir
else
TARGET=project1
RM=rm -f
MKDIR=mkdir -p
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
