SRC_DIR := src
THIRDPARTY_DIR := thirdparty

.PHONY: all
all: server.out client.out

server.out: $(SRC_DIR)/server.o $(SRC_DIR)/utils.o $(SRC_DIR)/serialize.o
	gcc $(SRC_DIR)/server.o $(SRC_DIR)/utils.o $(SRC_DIR)/serialize.o -o server.out

client.out: $(SRC_DIR)/client.o $(SRC_DIR)/utils.o $(SRC_DIR)/serialize.o
	gcc $(SRC_DIR)/client.o $(SRC_DIR)/utils.o $(SRC_DIR)/serialize.o -o client.out

$(SRC_DIR)/server.o: $(SRC_DIR)/server.c
	gcc -c $(SRC_DIR)/server.c -I$(THIRDPARTY_DIR) -g -o $(SRC_DIR)/server.o

$(SRC_DIR)/client.o: $(SRC_DIR)/client.c
	gcc -c $(SRC_DIR)/client.c -I$(THIRDPARTY_DIR) -g -o $(SRC_DIR)/client.o

$(SRC_DIR)/utils.o: $(SRC_DIR)/utils.c
	gcc -c $(SRC_DIR)/utils.c -I$(THIRDPARTY_DIR) -g -o $(SRC_DIR)/utils.o

$(SRC_DIR)/serialize.o: $(SRC_DIR)/serialize.c
	gcc -c $(SRC_DIR)/serialize.c -I$(THIRDPARTY_DIR) -g -o $(SRC_DIR)/serialize.o

.PHONY: clean
clean:
	rm -f **/*.o
	rm -f server.out
	rm -f client.out

.PHONY: test
test:
	# TODO: tests
