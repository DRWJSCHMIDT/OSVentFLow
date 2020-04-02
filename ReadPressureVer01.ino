/*
    Release 1.0   March 30, 2020 by Dr. Bill Schmidt-J68HZ.  This program ws written to interface a MP3V7010xx
                  gauge pressure sensor and will calculate instantaneous average pressures for the 
                  Inhale or exhale cycle.  See comments at end for integrating variables to be displayed into the LCD.
*/
#define DEBUG                true   // Turn on to debug

#define AVERAGEBINNUMBER        10  // Number of averaging bins for the averaging routine
#define PRESSURESENSORPIN     (A7)  // pin that the pressure sensor is attached to
#define VSOURCE                 3.0 //Source voltage for differential pressure sensor

#define PRESSUREREADDELAY       20L // wait 20 ms between reads

int   rawSensorValue = 0;        //Measured differential pressure sensor raw value in bits.
long  randomNumber;              // Use for testing
float pressureSensorVoltage;     //Measured gauge pressure sensor voltage
float gaugeP;                    //Measured gauge pressure sensor Flow Delta Pressure in Kpa
float averagePressure;           //Average pressure
float pConversion = 4.01463;     // Conversion for kPa to Inches of water 
int i, j;

/*****
    Purpose: To delay so many milliseconds without blocking interrupts

  Parameter list:
    unsigned long ms      the number of milliseconds to delay

  Return value:
    void

*****/
void MyDelay(unsigned long ms)
{
  unsigned long currentMillis = millis();
  unsigned long previousMillis = millis();

  while (currentMillis - previousMillis < ms) {
    currentMillis = millis();
  }
}


/*****
  Purpose: Calculate the  pressure, gaugeP, from the sensor-generated voltage

  Argument list:
    void

  Return value:
    float         the voltage value expressed as kPa

  CAUTION:
*****/
float ReadPressureSensor() 
{
#ifdef DEBUG                        // Fake a test value

  randomNumber          = random(0, 1023);                                        // Simulate a voltage reading  
  pressureSensorVoltage = ((float) map(randomNumber, 0, 1023, 0, 500)) / 100.0;   //Make it look like a voltage between 0-3V
  gaugeP                =  pConversion * ((pressureSensorVoltage / VSOURCE) -0.08) / 0.09; //Convert volts to P, Return gaugeP;
 
  Serial.print("ReadPressureSensor    ");
  Serial.print("randomNumber = ");
  Serial.print(randomNumber);
  Serial.print("   pressureSensorVoltage = ");
  Serial.print(pressureSensorVoltage);
  Serial.print("    gaugeP = ");
  Serial.println(gaugeP);  

  return gaugeP;
#endif
                                    // Do the real thing here
                  
  rawSensorValue        = analogRead(PRESSURESENSORPIN);                          //Raw digital input from pressure sensor
  pressureSensorVoltage = ((float) map(rawSensorValue, 0, 1023, 0, 500)) / 100.0;   //Convert raw bits read to volts
  gaugeP                = pConversion*((pressureSensorVoltage / VSOURCE) -0.08) / 0.09;  //Convert volts to Pa  Return gaugeP;
  return gaugeP;
}

/*****
  Purpose: Calculate the average change in pressure from gaugeP

  Argument list:
    void

  Return value:
    void

  CAUTION:
*****/
void CalculateAveragePressure() 
{
  averagePressure = 0.0;
  for (j = 0; j < AVERAGEBINNUMBER; j++) {
    averagePressure += ReadPressureSensor();
    MyDelay(PRESSUREREADDELAY);    
  }
  averagePressure /= (float) AVERAGEBINNUMBER;
#ifdef DEBUG
  Serial.print("CalculateAveragePressure");
  Serial.print("     averagePressure = ");
  Serial.println(averagePressure);      
#endif
}


//====================================================================
void setup()   {
#ifdef DEBUG
  Serial.begin(9600);          // Set the serial monitor
  Serial.println("\n=== SUBROUTINE SETUP ===");
  randomSeed(analogRead(A0)); 
#endif
  analogReference(DEFAULT);
}


void loop()
{
  CalculateAveragePressure();

  // CODEWRITER:  Here you should print out the averagePressure to the LCD Units are currently inches of water
  
#ifdef DEBUG
  Serial.println("\nMAIN LOOP");
  Serial.print("   AveragePressure = "); 
  Serial.print(averagePressure);
  #endif  
}
