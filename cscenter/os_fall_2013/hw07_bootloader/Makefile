all:
	gcc boot.s -o boot.bin -nostdlib -Wl,-e,_start,-Ttext,0x7c00,--oformat,binary
clean:
	rm boot.bin