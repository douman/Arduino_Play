//Number of pulses, used to measure energy.
// From: http://openenergymonitor.org/emon/buildingblocks/interrupt-based-pulse-counter
// drm 20141023
//
long pulseCount = 0;   
//Used to measure power.
unsigned long pulseTime,lastTime;

//power and energy
double power, elapsedkWh;

//Number of pulses per wh - found or set on the meter.
int ppwh = 1; //1000 pulses/kwh = 1 pulse per wh

void setup()
{
  Serial.begin(115200);

// KWH interrupt attached to IRQ 1  = pin3
  attachInterrupt(1, onPulse, FALLING);
}


void loop()
{

}

// The interrupt routine
void onPulse()
{

//used to measure time between pulses.
  lastTime = pulseTime;
  pulseTime = micros();

//pulseCounter
  pulseCount++;

//Calculate power
  power = (3600000000.0 / (pulseTime - lastTime))/ppwh;
  
  //Find kwh elapsed
    elapsedkWh = (1.0*pulseCount/(ppwh*1000)); //multiply by 1000 to pulses per wh to kwh convert wh to kwh

//Print the values.
  Serial.print(power,4);
  Serial.print(" ");
  Serial.println(elapsedkWh,3);
}


