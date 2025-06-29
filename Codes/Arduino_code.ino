#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Filters.h>
#define RS              PIN_PB0
#define EN              PIN_PD7
#define D4              PIN_PD6
#define D5              PIN_PD5
#define D6              PIN_PB7
#define D7              PIN_PB6
#define Relay           PIN_PC1
#define Phase_Voltage   PIN_PC0
#define ONE_WIRE_BUS    PIN_PB2
#define Buzzer          PIN_PB1
#define Relay           PIN_PC1

char buf[17];
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
float testFrequency = 50;
float windowLength = 40.0 / testFrequency;
float intercept = -0.04;
float Slope_Voltage = 1.40;
unsigned long printPeriod           = 1000;
unsigned long previousMillis        = 0;
int Voltage_Analog_Sample           = 0;
unsigned int Voltage_Digital_Sample = 0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() 
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  sensors.begin(); 
  pinMode(Buzzer, OUTPUT);
  pinMode(Relay,  OUTPUT);
}

void loop() 
{
  RunningStatistics inputStats1;  //Easy life lines, actual calculation of the RMS requires a load of coding
  inputStats1.setWindowSecs(windowLength);
  while(1)
  {
    Voltage_Analog_Sample    = analogRead(Phase_Voltage);
    inputStats1.input(Voltage_Analog_Sample);  
    if((unsigned long)(millis() - previousMillis) >= printPeriod) 
    {
      previousMillis = millis();
      Voltage_Digital_Sample = intercept + Slope_Voltage * inputStats1.sigma();
      Voltage_Digital_Sample = map(Voltage_Digital_Sample, 310, 430, 230, 240);
      sensors.requestTemperatures();
      float temperatureC = sensors.getTempCByIndex(0);
      if((Voltage_Digital_Sample >= 200) && (Voltage_Digital_Sample <= 260))
      {
        lcd.setCursor(0, 0);
        sprintf(buf, "Voltage:%3d Volt", Voltage_Digital_Sample);
        lcd.print(buf);
        lcd.setCursor(0, 1);
        lcd.print("Temp:");
        lcd.print(temperatureC);
        if(temperatureC >= 35)
        {
          lcd.setCursor(0, 0);
          lcd.print("HIGH Tempature ");
          lcd.setCursor(0, 1);
          lcd.print("tor Off.........");
          digitalWrite(Buzzer, HIGH);
          digitalWrite(Relay,  LOW);
        }
        digitalWrite(Buzzer, LOW);
        digitalWrite(Relay,  HIGH);
      }
      else if(Voltage_Digital_Sample >= 260)
      {
        lcd.setCursor(0, 0);
        lcd.print("HIGH Voltage Mo-");
        lcd.setCursor(0, 1);
        lcd.print("tor Off.........");
        digitalWrite(Buzzer, HIGH);
        digitalWrite(Relay,  LOW);
      }
      else if(Voltage_Digital_Sample <= 200)
      {
        lcd.setCursor(0, 0);
        lcd.print("LOW Voltage Mo- ");
        lcd.setCursor(0, 1);
        lcd.print("tor Off.........");
        digitalWrite(Buzzer, HIGH);
        digitalWrite(Relay,  LOW);
      }
      sendMultipleData(temperatureC, Voltage_Digital_Sample);
    }
  }
}

void sendMultipleData(float intValue1, int intValue2) 
{
  Serial.print("<");
  Serial.print(intValue1);    //Value1
  Serial.print(",");
  Serial.print(intValue2);    //Value2
  Serial.print(",");
  Serial.println(">");
}
