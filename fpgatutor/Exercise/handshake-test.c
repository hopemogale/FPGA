// handshake-test.c 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>


int main(int argc, char *argv[])
{
  int loop,max_loop,j;
  unsigned int sigBusy, sigError;;
  
  wiringPiSetupGpio();

  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(24, INPUT);
  pinMode(25, INPUT);

  max_loop = 1000;
  if (argc==2) max_loop = atoi(argv[1]);

  digitalWrite(21, LOW); // not Go
  digitalWrite(20, HIGH); // Reset
  digitalWrite(20, LOW);

  for(loop=0;loop<max_loop;loop++) {
    delay(1);
    digitalWrite(21, HIGH); // Go
    sigBusy = digitalRead(24);
    if (sigBusy==HIGH) {
      digitalWrite(21, LOW); // not Go
    }
    sigError = digitalRead(25);
    if(sigError==HIGH) {
      digitalWrite(21, LOW); // not Go
      printf("Error @loop=%d\n",loop);
      digitalWrite(20, HIGH); // Reset
      digitalWrite(20, LOW);
    }
  }
  return 0;
}
