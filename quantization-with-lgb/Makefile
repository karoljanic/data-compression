CC := clang++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic -O3

all: clean quantization

quantization:
	$(CC) $(CFLAGS) ../tga/tga.cpp main.cpp -o quantization
	
clean:
	rm -f quantization
