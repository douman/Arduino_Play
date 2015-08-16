void setup() 
{
  drm_read_rom(id);
  new nrf24();
  nrf24.rec_addr = id.serial;
  nrf24.rf_chan = id.chan;  
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
