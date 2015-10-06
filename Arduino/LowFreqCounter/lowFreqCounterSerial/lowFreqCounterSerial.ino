// INPUT: interrupt 0 (PIN2)

long previousMillis = 0;
volatile unsigned long duration=0;
volatile unsigned int pulsecount=0;
volatile unsigned long prevMicros=0;

void setup()
{
  Serial.begin(9600);
  attachInterrupt(0, periodIrq, RISING);
}

void loop()
{
  Serial.print(duration,DEC);
  Serial.print("uS      ");
  Serial.print(1e6/duration,2);
  Serial.println("Hz      ");
  delay(100);
}

void periodIrq() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration = currentMicros - prevMicros;
  prevMicros = currentMicros;
}
