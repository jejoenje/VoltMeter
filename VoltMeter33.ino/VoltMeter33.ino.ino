/**
 * 3.3v Volt meter
 */

#define NUM_SAMPLES 50

unsigned char sample_count = 0;   
float refvcc = 0;
float sum = 0;


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

void setup() {
 Serial.begin(9600); //BaudRate
 delay(100);

}

void loop() {
   while (sample_count < NUM_SAMPLES) {
     refvcc = readVcc()/float(1024.00);
     sum += refvcc;
     sample_count++;
     delay(10);
    }
    refvcc = sum/float(NUM_SAMPLES);
    sum = 0;
    sample_count = 0;

  Serial.print("refvcc = ");
  Serial.print(refvcc);
  Serial.println();
}
