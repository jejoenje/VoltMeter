# VoltMeter

This build is an Arduino-based volt meter which displays voltage to a 7 segment (four digits) display.
It can measure its own voltage source and incorporates a VOLT_CORRECTION factor which can be used to 
calibrate the voltage readings against an external meter.
Also includes a LDR being read across a 100k resistor voltage divider; this reading is used to set the brightness of 
the display (in three steps), to make sure the display is readable in different light conditions and not too blinding.
