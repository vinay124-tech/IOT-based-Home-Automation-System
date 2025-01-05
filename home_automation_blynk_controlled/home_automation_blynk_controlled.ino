/*************************************************************
Title         :   Home automation using Blynk
Description   :   To control light's brightness, monitor temperature, monitor water level in the tank through Blynk app
Pheripherals  :   Arduino UNO, Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
*************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "Enter your blynk template ID here"
#define BLYNK_TEMPLATE_NAME "Home Automation"
#define BLYNK_AUTH_TOKEN "enter your blynk authentication token here"

// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw, inlet_sw, outlet_sw, cooler_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  cooler_sw = param.asInt(); // Use the global cooler_sw variable
  if (cooler_sw)
  {
    cooler_control(ON);
    lcd.setCursor(8, 0);
    lcd.print("CLR_ON ");
  }
  else
  {
    cooler_control(OFF);
    lcd.setCursor(8, 0);
    lcd.print("CLR_OFF");
  }
}

/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN)
{
  heater_sw = param.asInt(); // Use the global heater_sw variable
  if (heater_sw)
  {
    heater_control(ON);
    lcd.setCursor(8, 0);
    lcd.print("HTR_ON ");
  }
  else
  {
    heater_control(OFF);
    lcd.setCursor(8, 0);
    lcd.print("HTR_OFF");
  }
}

/*To turn ON and OFF inlet valve based on virtual PIN value*/
/*To turn ON and OFF inlet valve based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  inlet_sw = param.asInt();
  if (inlet_sw)
  {
    enable_inlet(); // Turn on the inlet
    lcd.setCursor(8, 1);
    lcd.print("IN_FL_ON ");
  }
  else
  {
    disable_inlet(); // Turn off the inlet (corrected)
    lcd.setCursor(8, 1);
    lcd.print("IN_FL_OFF");
  }
}


/*To turn ON and OFF outlet valve based on virtual PIN value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt(); // Use the global outlet_sw variable
  if (outlet_sw)
  {
    enable_outlet();
    lcd.setCursor(8, 1);
    lcd.print("OT_FL_ON ");
  }
  else
  {
    disable_outlet(); // Correctly disable outlet when switched off
    lcd.setCursor(8, 1);
    lcd.print("OT_FL_OFF");
  }
}

/* To display temperature and water volume as gauge on the Blynk App */
void update_temperature_reading()
{
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
  if (read_temperature() > 35.0 && heater_sw)
  {
    heater_sw = 0;
    heater_control(OFF);
    Blynk.virtualWrite(HEATER_V_PIN, 0);
    lcd.setCursor(8, 0);
    lcd.print("HTR_OFF");
  }
}

/*To control water volume above 2000 liters*/
void handle_tank(void)
{
  if (tank_volume < 2000 && inlet_sw == 0)
  {
    enable_inlet();
    inlet_sw = 1;
    Blynk.virtualWrite(INLET_V_PIN, 1);
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON ");
  }
  if (tank_volume == 3000 && inlet_sw == 1) 
  {
    disable_inlet();
    inlet_sw = 0;
    Blynk.virtualWrite(INLET_V_PIN, 0);
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");
  }
}

void setup(void)
{
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();

  lcd.setCursor(0, 0);
  lcd.print("Home Automation");

  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T=");

  Blynk.begin(BLYNK_AUTH_TOKEN);

  init_ldr();
  init_temperature_system();
  init_serial_tank();

  timer.setInterval(1000, update_temperature_reading);

}

String temp;

void loop(void)
{
  brightness_control();
  temp = String(read_temperature(), 2);

  lcd.setCursor(2, 0);
  lcd.print(temp);

  tank_volume = volume();
  lcd.setCursor(2, 1);
  lcd.print(tank_volume);
  
  Blynk.run();
  timer.run();
  handle_temp();
  handle_tank();
  
}
