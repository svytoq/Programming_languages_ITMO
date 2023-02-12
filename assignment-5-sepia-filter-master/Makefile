CFLAGS = -g -O2 -Wall -Werror -std=c17 -Wdiscarded-qualifiers -Wincompatible-pointer-types -Wint-conversion

all: build/bmp.o build/image.o build/transformer.o build/main.o build/header.o build/sepia.o
	gcc -no-pie -o build/transformer $^

build/%.o: src/%.c
	mkdir -p build
	gcc -c $(CFLAGS) -o $@ $<

build/%.o: src/%.asm
	mkdir -p build
	nasm -felf64 -o $@ $<

clean:
	rm -rf build

.phony: clean
