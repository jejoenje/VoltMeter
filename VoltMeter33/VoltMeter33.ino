#include <TM1637.h>


 /**
 * 3.3v Volt meter
 */

//#include <TM1637Display.h>


#define NUM_SAMPLES 50
#define CLK1 9
#define DIO1 8
#define CLK2 6
#define DIO2 5
#define VOLT_CORRECTION 2.0

TM1637Display display1(CLK1, DIO1);
TM1637Display display2(CLK2, DIO2);


int analogInput1 = A1;
int analogInput2 = A2;
int lightInput = A0;
float R1 = 99500.00; // resistance of R1 (100K) 
float R2 = 9970.00; // resistance of R2 (10K) 

unsigned char sample_count = 0;   
float refvcc = 0;
float sum = 0;
float sum1 = 0;
float sum2 = 0;
int val1 = 0;
int val2 = 0;
float Vout1 = 0.00;
float Vout2 = 0.00;
float Vin1 = 0.00;
float Vin2 = 0.00;
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

float getVoltage(int measurePin) {
  
}

void setup() {
 pinMode(analogInput1, INPUT); //assigning the input port
 pinMode(lightInput, INPUT); //assigning the input port
 Serial.begin(9600); //BaudRate
 light = adjustBrightness();
 display1.setBrightness(light);
 display2.setBrightness(light);
 delay(100);
 
}

void loop() {

  light = adjustBrightness();
  display1.setBrightness(light);
  display2.setBrightness(light);
  
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
    sum1 += analogRead(analogInput1);
    sum2 += analogRead(analogInput2);
    sample_count++;
    delay(10);
  }
  sample_count = 0;

  val1 = float(sum1)/float(NUM_SAMPLES);
  val2 = float(sum2)/float(NUM_SAMPLES);
  Vout1 = (val1 * refvcc) / 1024.00; // formula for calculating voltage out i.e. V+, here 5.00
  Vout2 = (val2 * refvcc) / 1024.00;
  Vin1 = Vout1 / (R2/(R1+R2)); // formula for calculating voltage in i.e. GND
  Vin2 = Vout2 / (R2/(R1+R2));
  if (Vin1<0.09)//condition 
  {
    Vin1=0.00;//statement to quash undesired reading !
  } 
  if (Vin2<0.09)//condition 
  {
    Vin2=0.00;//statement to quash undesired reading !
  } 

  Vin1 = Vin1 + (Vin1/float(100))*float(VOLT_CORRECTION);
  Vin2 = Vin2 + (Vin2/float(100))*float(VOLT_CORRECTION);
  
  whole = Vin1;
  rem = (Vin1-whole)*100;
  //display1.setColonOn(1);
  display1.showNumberDecEx(whole, (0x80 >> 3), false, 2, 0);
  display1.showNumberDecEx(rem, 0, true, 2, 2);

  whole = Vin2;
  rem = (Vin2-whole)*100;
  display2.showNumberDecEx(whole, (0x80 >> 3), false, 2, 0);
  display2.showNumberDecEx(rem, 0, true, 2, 2);

  sample_count=0;
  sum=0;
  sum1=0;
  sum2=0;
  val1=0;
  val2=0;
  Vin1=0;
  Vin2=0;
  Vout1=0;
  Vout2=0;
  whole = 0;
  rem = 0;

  delay(50);
}
