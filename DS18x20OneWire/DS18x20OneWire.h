int triggerAllDS(OneWire ds)
{
    ds.write(0xcc, 1); // skip ROM, address all units
    ds.write(0x44, 1); // convert temperature (all units)
    return(ST_AOK);
}

int ReadNextDS(OneWire ds, float tempC)
{
  
}


