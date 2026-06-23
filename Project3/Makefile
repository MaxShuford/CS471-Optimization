CC=gcc
CFLAGS=-O2 -std=c11 -Wall -Wextra -pedantic -g -Iinclude
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
     $(SRC_DIR)/timing.c\
	$(SRC_DIR)/LS.c \
     $(SRC_DIR)/DE.c \
     $(SRC_DIR)/PSO.c \
     $(SRC_DIR)/BS.c \
     $(SRC_DIR)/clamp.c \
     $(SRC_DIR)/rand.c


OBJS=$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ifeq ($(OS),Windows_NT)
TARGET=project3.exe
RM=del /Q
MKDIR=mkdir
else
TARGET=project3
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