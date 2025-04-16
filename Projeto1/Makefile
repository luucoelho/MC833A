CC = gcc
CFLAGS = -Wall -Wextra -pthread

# detect operating system
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS specific settings
    CFLAGS += -I/opt/homebrew/include
endif

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SERVER_SOURCES = $(SRC_DIR)/server.c $(SRC_DIR)/serialization.c $(SRC_DIR)/jsonManager.c
CLIENT_SOURCES = $(SRC_DIR)/client.c $(SRC_DIR)/serialization.c

SERVER_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SERVER_SOURCES))
CLIENT_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CLIENT_SOURCES))

all: directories server client

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

server: $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^

client: $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean directories 