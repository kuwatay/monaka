#include <SPI.h>
#include <LiquidCrystal.h>

// for normal LCD library
// LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  

// for SPI interface LCD library
// http://playground.arduino.cc/Main/LiquidCrystal
LiquidCrystal lcd(10); 

long previousMillis = 0;
volatile unsigned long duration=0;
volatile unsigned int pulsecount=0;
volatile unsigned long prevMicros=0;
volatile uint8_t handleIrq = 0;

void setup()
{
  lcd.begin(16, 2);  
  attachInterrupt(0, periodIrq, RISING); // D2
}

void loop()
{
  if (handleIrq == 0) {
    lcd.setCursor(0, 0);
    lcd.print("OUT OF RANGE");
    lcd.setCursor(0, 1);
    lcd.print(duration,DEC);
    lcd.print("          ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print(duration,DEC);
    lcd.print("uS        ");
    lcd.setCursor(0, 1);
    lcd.print(1e6/duration,2);
    lcd.print("Hz       ");
    handleIrq = 0;
  }
  delay(100);
}

void periodIrq() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration = currentMicros - prevMicros;
  prevMicros = currentMicros;
  handleIrq = 1;
}
