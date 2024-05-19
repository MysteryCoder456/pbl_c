SRC_DIR := src

.PHONY: all
all: server.out client.out

server.out: $(SRC_DIR)/server.o $(SRC_DIR)/utils.o
	gcc $(SRC_DIR)/server.o $(SRC_DIR)/utils.o -o server.out

client.out: $(SRC_DIR)/client.o $(SRC_DIR)/utils.o
	gcc $(SRC_DIR)/client.o $(SRC_DIR)/utils.o -o client.out

$(SRC_DIR)/server.o: $(SRC_DIR)/server.c
	gcc -c $(SRC_DIR)/server.c -g -o $(SRC_DIR)/server.o

$(SRC_DIR)/client.o: $(SRC_DIR)/client.c
	gcc -c $(SRC_DIR)/client.c -g -o $(SRC_DIR)/client.o

$(SRC_DIR)/utils.o: $(SRC_DIR)/utils.c
	gcc -c $(SRC_DIR)/utils.c -g -o $(SRC_DIR)/utils.o

.PHONY: clean
clean:
	rm -f **/*.o
	rm -f server.out
	rm -f client.out
