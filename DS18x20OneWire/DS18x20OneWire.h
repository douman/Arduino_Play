int triggerAllDS(OneWire ds)
{
    ds.write(0xcc, 0); // skip ROM, address all units
    delay(100);
    ds.write(0x44, 0); // convert temperature (all units)
    delay(100);
    return(ST_AOK);
}

int ReadNextDS(OneWire ds, float tempC)
{
  
}


