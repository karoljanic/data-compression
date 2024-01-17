CC := clang++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic -O3

all: encoder decoder compare

encoder:
	$(CC) $(CFLAGS) ../tga/tga.cpp encoder.cpp -o encoder

decoder:
	$(CC) $(CFLAGS) ../tga/tga.cpp decoder.cpp -o decoder

compare:
	$(CC) $(CFLAGS) ../tga/tga.cpp compare.cpp -o compare

clean:
	rm -f encoder decoder compare
