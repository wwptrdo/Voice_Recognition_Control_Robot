

CC = gcc
CFLAGS = -Iinclude -lpthread -lwiringPi -lrt -ldl -lpthread -lasound

TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src
BIN_PATH = $(TOP_PATH)/bin
LIB_PATH = $(TOP_PATH)/lib

all: $(BIN_PATH)/test $(BIN_PATH)/test_old

$(BIN_PATH)/test: $(LIB_PATH)/voice.o src/main.c
	$(CC) $(CFLAGS) -o $(BIN_PATH)/test $(LIB_PATH)/libmsc.so $(LIB_PATH)/voice.o src/main.c

$(BIN_PATH)/test_old: $(LIB_PATH)/voice_old.o src/main_old.c
	$(CC) $(CFLAGS) -o $(BIN_PATH)/test_old $(LIB_PATH)/libmsc.so $(LIB_PATH)/voice_old.o src/main_old.c

$(LIB_PATH)/voice.o: src/voice.c
	$(CC) $(CFLAGS) -o lib/voice.o -c src/voice.c

$(LIB_PATH)/voice_old.o: src/voice_old.c
	$(CC) $(CFLAGS) -o lib/voice_old.o -c src/voice_old.c

clean:
	rm -rf lib/*.o bin/* 
