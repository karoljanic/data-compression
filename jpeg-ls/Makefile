CC := clang++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic -O3

all: clean jpeg-ls

jpeg-ls:
	$(CC) $(CFLAGS) ../tga/tga.cpp main.cpp -o jpeg-ls
	
clean:
	rm -f jpeg-ls
