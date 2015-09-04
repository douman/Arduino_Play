/*
 nRF24L01+ code to test transmit and recieve from the same arduino

 by drm 20150827 
 using pro mini 3.3V at 8MHz
 using Adafruit breadboard powersupply 3.3 for nRF24L01+ modules
 
 Wired up as follows:
 Pin  Function      Wirecolor  xmitunit  recunit  Comment
 1    GND           Black      0V        0V        From PS
 2    VCC           Red        3.3V      3.3V      From PS
 3    CE/(Rx/Tx)    Brown      #5        #7
 4    CSN/Chip Sel  Green      #4        #6
 5    SCK/Clock     Blue       #13       #13       Dedicated SPI SCK
 6    MOSI/Slv In   Yellow     #11       #11       Dedicated SPI MOSI
 7    MISO/Slv Out  White      #12       #12       Dedicated SPI MISO
 8    IRQ/Interupt  Orange     #2        #3        Pro Mini interupts
 
 */
#define CETX 5 
#define CERX 7
#define CSRX 6
#define CSTX 4
#define MISO 12
#define MOSI 11
#define SCL 13
#define IRQRX 3
#define IRQTX 2
#define TX LOW
#define RX HIGH

void setup() 
{
  drm_read_rom(id);
  new nrf24_rec();
  new nrf24_xmit();
  nrf24_rec.rec_addr = id.serial;
  nrf24_rec.rf_chan = id.chan;  
  // put your setup code here, to run once:
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if(nrf24.available)
  {
    Serial.println(nrf23.data);
  }
}
