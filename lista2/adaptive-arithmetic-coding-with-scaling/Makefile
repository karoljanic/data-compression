CC := clang++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic

all: compress decompress

encoder:
	$(CC) $(CFLAGS) compress.cpp -o compress

decoder:
	$(CC) $(CFLAGS) decompress.cpp -o decompress

clean:
	rm -f compress decompress
