#define ONTIME 10
#define OFFTIME 80
/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
  */

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int j;
// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  j=0;
}

// the loop routine runs over and over again forever:
void loop() {
  int i;
#ifdef OLD  
  for (i=0;i<5;i++)
  {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    if (j % 2 == 0) 
    {
      delay(100+i*100);               // wait for a second
    }
    else
    {
      delay(OFFTIME);               // wait for a second
    }
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    if (j % 2 == 0)
    {
      delay(10+i*200);               // wait for a second
    }
    else
    {
      delay(ONTIME);               // wait for a second
    }
  }
  for (i=0;i<10;i++) {digitalWrite(led, HIGH); delay(50); digitalWrite(led, LOW); delay(50);}
  j++;
#else
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(ONTIME);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(OFFTIME);              // wait for a second
#endif

}

