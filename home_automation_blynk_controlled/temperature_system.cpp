#include "temperature_system.h"
#include "Arduino.h"
#include "main.h"


void init_temperature_system(void)
{
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(TEMPERATURE_SENSOR, INPUT);    
}


float read_temperature(void)
{
  float temperature;
  temperature = ((analogRead(A0)*(float)5/1024)/((float)0.01));
  return temperature;
}

void cooler_control(bool control)
{
  if(control)
  {
    digitalWrite(COOLER, HIGH);
  }
  else
  {
    digitalWrite(COOLER, LOW);
  }
   
}


void heater_control(bool control)
{
  if(control)
  {
    digitalWrite(HEATER, HIGH);
  }
  else
  {
    digitalWrite(HEATER, LOW);
  }
    
}
