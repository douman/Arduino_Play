#include "M0_GPS.h"
// Output Functions for M0_DiveLogger Project
int Output_9DoF()
{
  // Write the data
  sensors_event_t accel, mag, gyro, temp; // all local, no global knowledge of 9 DoF
  my9DOF.getEvent(&accel, &mag, &gyro, &temp); 
  String out = String(OUT_SIZE);

  out = String(log_cnt++) + "\tacl\t" + String(accel.acceleration.x, 2) + "\t" + 
                                        String(accel.acceleration.y, 2) + "\t" + 
                                        String(accel.acceleration.z, 2) + "\t" +
                                        String(sqrt(accel.acceleration.x * accel.acceleration.x + 
                                                    accel.acceleration.y * accel.acceleration.y + 
                                                    accel.acceleration.z * accel.acceleration.z), 2) + "\r\n" +
        String(log_cnt++) + "\tgyr\t" + String(gyro.gyro.x, 2) + "\t" + 
                                        String(gyro.gyro.y, 2) + "\t" + 
                                        String(gyro.gyro.z, 2) + "\t" +
                                        String(sqrt(gyro.gyro.x * gyro.gyro.x + 
                                                    gyro.gyro.y * gyro.gyro.y + 
                                                    gyro.gyro.z * gyro.gyro.z), 2) + "\r\n" +
        String(log_cnt++) + "\tmag\t" + String(mag.magnetic.x, 2) + "\t" + 
                                        String(mag.magnetic.y, 2) + "\t" + 
                                        String(mag.magnetic.z, 2) + "\t" +
                                        String(sqrt(mag.magnetic.x * mag.magnetic.x + 
                                                    mag.magnetic.y * mag.magnetic.y + 
                                                    mag.magnetic.z * mag.magnetic.z), 2);
  if(serprt) Serial.println(out);
  if(bleprt) Serial.println(out);
  return(ST_AOK);
}

//// print out the SAMD processor serial number
//void print_samM0_serial()
//{
//  long s[4], *ser;
//  ser = (long *) 0x0080A00C; // address of the processor serial number
//  int i;
//  for (i=3; i>=0; i--) s[i] = *ser++;
//
//  if(serprt) Serial.print("s ");
//  for(i=0; i<4; i++) if(serprt) Serial.print(s[i], HEX);
//  if(serprt) Serial.println();
//}

int printXY()
{
  if(serprt)
  {
    String out = String(OUT_SIZE); // string for building the outptu string for GPS
    // Format and print the x, y integrated coords
    
    out = String(log_cnt++) + "\txyz\t" + String(x_sum) + "\t" + String(y_sum);
    if(serprt)
    {
      if(serprt) Serial.println(out);
    }
    if(wrt_ble && (ppscnt % BLEMOD == 0 && ppscnt > 10))
    {
      bleprt=true;
      ble.println(out);
    }
  }
}
