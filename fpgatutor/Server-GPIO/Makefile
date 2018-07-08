all: server-GPIO

server-GPIO: server-GPIO.c mongoose.c
	gcc $^ -o $@ -g -lwiringPi

clean:
	rm -rf *.o *~ server-GPIO
