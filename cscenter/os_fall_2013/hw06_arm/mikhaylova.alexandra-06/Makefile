all:
	gcc -shared hello.s -o libhello.so
	gcc -o hello hello.c -L. -lhello

clean:
	rm libhello.so hello