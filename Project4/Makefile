CC=gcc
CFLAGS=-Wall -Wextra -pedantic -g -Iinclude
LDFLAGS=-lm

SRC_DIR=src
OBJ_DIR=build
INCLUDE_DIR=include
OUTPUT_DIR = Results_471
SCHEDULES_DIR = Results_471/schedules

SRCS=$(SRC_DIR)/main.c \
     $(SRC_DIR)/mt19937ar.c \
     $(SRC_DIR)/timing.c\
     $(SRC_DIR)/fileutility.c \
     $(SRC_DIR)/flowshop.c \
     $(SRC_DIR)/blocking.c \
	 $(SRC_DIR)/config.c\
	 $(SRC_DIR)/ACO.c 


OBJS=$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

ifeq ($(OS),Windows_NT)
TARGET=project4.exe
RM=del /Q
MKDIR=mkdir
else
TARGET=project4
RM=rm -f
MKDIR=mkdir -p
endif

all: $(TARGET)

$(TARGET): $(OBJS) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)
$(OUTPUT_DIR):
	$(MKDIR) $(OUTPUT_DIR)
	$(MKDIR) $(SCHEDULES_DIR)

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)