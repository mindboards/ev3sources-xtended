# Configurable module loading test program
#
# Changelog:
# - 0.1: Initial release
#
# License: restricted for now
#
# Xander Soldaat <xander@robotc.net>

all: robotc.so labview.so main

labview.so: labview.c
	gcc -shared -o labview.so -fPIC labview.c
	
robotc.so: robotc.c
	gcc -shared -o robotc.so -fPIC robotc.c
	
main: main.c
	gcc -o main main.c -ldl
	
clean:
	rm -f *.so *.o main
