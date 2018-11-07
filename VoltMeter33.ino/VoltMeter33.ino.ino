/**
 * 3.3v Volt meter
 */

#include <TM1637Display.h>

#define NUM_SAMPLES 50
#define CLK 9
#define DIO 8
#define VOLT_CORRECTION 5.5

TM1637Display display(CLK, DIO);

int analogInput = A1;
int lightInput = A0;
float R1 = 99500.00; // resistance of R1 (100K) 
float R2 = 9970.00; // resistance of R2 (10K) 

unsigned char sample_count = 0;   
float refvcc = 0;
float sum = 0;
int val = 0;
float Vout = 0.00;
float Vin = 0.00;
int whole = 0;
int rem = 0;
int light = 2;
int sensorValue = 0;
int defaultLight = 2;

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  //result = 1195700UL / (unsigned long)result; //1126400 = 1.1*1024*1000
  return result;
}

int adjustBrightness() {

  float currentMeasure = 0;
  int measureCount = 0;
  defaultLight = 2;

  while (measureCount < NUM_SAMPLES) {
    currentMeasure += analogRead(lightInput);
    measureCount++;
    delay(10);
  }
  sensorValue = currentMeasure/(NUM_SAMPLES);

  if(sensorValue < 15) {
    defaultLight = 3;
  }
  if(sensorValue > 90) {
    defaultLight = 1;
  }

  return(defaultLight);
}

void setup() {
 pinMode(analogInput, INPUT); //assigning the input port
 pinMode(lightInput, INPUT); //assigning the input port
 Serial.begin(9600); //BaudRate
 light = adjustBrightness();
 display.setBrightness(light);
 delay(1000);
}

void loop() {

  light = adjustBrightness();
  display.setBrightness(light);
  
  while (sample_count < NUM_SAMPLES) {
    refvcc = readVcc()/float(1024.00);
    sum += refvcc;
    sample_count++;
    delay(10);
  }
  refvcc = sum/float(NUM_SAMPLES);
  sum = 0;
  sample_count = 0;

  while (sample_count < NUM_SAMPLES) {
    sum += analogRead(analogInput);
    sample_count++;
    delay(10);
  }
  sample_count = 0;

  val = float(sum)/float(NUM_SAMPLES);
  Vout = (val * refvcc) / 1024.00; // formula for calculating voltage out i.e. V+, here 5.00
  Vin = Vout / (R2/(R1+R2)); // formula for calculating voltage in i.e. GND
  if (Vin<0.09)//condition 
  {
    Vin=0.00;//statement to quash undesired reading !
  } 

  Vin = Vin + (Vin/float(100))*float(VOLT_CORRECTION);

  whole = Vin;
  rem = (Vin-whole)*100;

  display.showNumberDecEx(whole, (0x80 >> 3), false, 2, 0);
  display.showNumberDecEx(rem, 0, true, 2, 2);

  Serial.print("refvcc = ");
  Serial.print(refvcc);
  Serial.print(", sum = ");
  Serial.print(sum);
  Serial.print(", val = ");
  Serial.print(val);
  Serial.print(", vcc = ");
  Serial.print(Vin);
  Serial.print(", sensor = ");
  Serial.print(sensorValue);
  Serial.print(", light = ");
  Serial.print(light);
  Serial.println();

  sample_count=0;
  sum=0;
  val=0;
  whole = 0;
  rem = 0;

  delay(1000);
}
