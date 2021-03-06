/**
 * BASIC VOLT METER SET UP
 * 
 * Reads voltage from pin A2
 * Displays using 7 segment display
 * 
 */

#include <TM1637Display.h>

#define NUM_SAMPLES 50
#define VOLT_CORRECTION 4.25
// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 8

TM1637Display display(CLK, DIO);

int analogInput = 1;
int lightInput = 0;

float Vout = 0.00;
float Vin = 0.00;
float refvcc = 0;
float test = 0;
float R1 = 99500.00; // resistance of R1 (100K) 
float R2 = 9970.00; // resistance of R2 (10K) 
int val = 0;
float sum = 0;
unsigned char sample_count = 0;     
int k;
int whole = 0;
int rem = 0;
int sensorValue = 0;
int light = 2;
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

  if(sensorValue < 800) {
    defaultLight = 1;
  }
  if(sensorValue > 990) {
    defaultLight = 3;
  }

  return(defaultLight);
}

void setup(){
   pinMode(analogInput, INPUT); //assigning the input port
   pinMode(lightInput, INPUT); //assigning the input port
   
   Serial.begin(9600); //BaudRate
   // Brightness: Appears to be 1-3 only
   light = adjustBrightness();
   display.setBrightness(light);
   delay(1000);
}

void loop(){
//   refvcc = readVcc()/float(1024.00);
//   refvcc = refvcc/1024;
//   refvcc = 3.90;

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
  
  Serial.print("V = ");
  Serial.print(Vin);
  Serial.print(", ref = ");
  Serial.print(refvcc);
  Serial.print(", test = ");
  Serial.print(test);
  Serial.print(", brightval = ");
  Serial.print(light);
  Serial.println();

  display.showNumberDecEx(whole, (0x80 >> 3), false, 2, 0);
  display.showNumberDecEx(rem, 0, true, 2, 2);
  
  delay(2000); //for maintaining the speed of the output in serial moniter

  sum=0;
  val=0;
  sample_count=0;

} 
