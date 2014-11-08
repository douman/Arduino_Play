// example code using mthread library
// slightly modified by drm 20141025
// -*- mode: c++ -*-

// Arduino-Compatible Multi-Threading Library (mthread)

// Copyright (C) 2010-2012 Jonathan Lamothe <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.

// * * *

// This program creates 5 seperate threads which report over serial
// when they're called.  It can be tested by running the program and
// watching the serial console.

#include <mthread.h>
#include <EEPROM.h>
const char *version="mthread_seaial_test -> V1.0.0-20141025";
unsigned long start_micros;

// Our custom Thread:
class FooThread : public Thread
{
public:
    FooThread(int id);
protected:
    bool loop();
private:
    int id;
};

FooThread::FooThread(int id)
{
    this->id = id;
}

bool FooThread::loop()
{

    // Die if requested:
    if(kill_flag)
        return false;

    // Print the status message:
    if(id==1) {
      unsigned long mic = micros() - start_micros;
      Serial.print(mic/1000000); Serial.print(".");
      mic =  1000000 + mic % 1000000;
      char str[8];
      sprintf(str,"%lu",mic);
      Serial.println(str+1);
      mic = mic - 1000000;
      if (mic > 500000) mic = 0;
      sleep_micro(id*1000000 - (mic+475));
    }
    else {
      sleep_micro(id*1000000);
    }
    
    Serial.print("T-> ");
    Serial.println(id);
    return true;
}

void setup()
{

    // Create five threads and add them to the main ThreadList:
    for(int i = 1; i <= 5; i++)
        main_thread_list->add_thread(new FooThread(i));

    // Initialize the serial connection:
    Serial.begin(115200);
    Serial.print(version); Serial.print(" - SN#");
    Serial.println(drm_serialno());
    delay(500);
    start_micros = micros();
}

// jl
short drm_serialno() {
  return(EEPROM.read(5) << 8 | EEPROM.read(6));
}
