CC := clang++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic -O3

all: encoder decoder

encoder:
	$(CC) $(CFLAGS) encoder.cpp -o encoder

decoder:
	$(CC) $(CFLAGS) decoder.cpp -o decoder

clean:
	rm -f encoder decoder
