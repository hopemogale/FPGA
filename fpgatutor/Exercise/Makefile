all: serial-test handshake-test

serial-test: serial-test.c
	gcc $^ -o $@ -g -lwiringPi

handshake-test: handshake-test.c
	gcc $^ -o $@ -g -lwiringPi

clean:
	rm -rf *.o *~ serial-test handshake-test
