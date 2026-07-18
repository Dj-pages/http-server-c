CC = gcc
CFLAGS = -Wall -Wextra -g

SRC_DIR = src
OUT_DIR = $(SRC_DIR)

TARGET = $(OUT_DIR)/server

HEADERS = $(SRC_DIR)/response.h \
          $(SRC_DIR)/status_code.h \
					$(SRC_DIR)/clint.h

$(TARGET): $(SRC_DIR)/server.c $(HEADERS)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(SRC_DIR)/server.c -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean
