/*
    Release 1.1   Equations from Dr. Bill Schmidt, code implemented by Jack Purdum, W8TEE
    Release 1.0   March 30, 2020 by Dr. Bill Schmidt-J68HZ.  This program ws written to interface a MPXV2007DP
                  differential pressure sensor and will calculate instantaneous average flow rates and total volumes
                  per Inhale or exhale cycle.  See comments at end for integrating variables to be displayed into the LCD.
*/
#define DEBUG                true   // Turn on to debug

#define AVERAGEBINNUMBER        10  // Number of averaging bins for the averaging routine
#define PRESSURESENSORPIN     (A7)  // pin that the flow pressure sensor is attached to
#define FLOWMAXSCALE            90  //Flow rate design Parameter is 90 Liters/min
#define VSOURCE                 5.0 //Source voltage for differential pressure sensor
#define PTFCC                63.639 //Conversion factor for this orifice.
#define MF                      1.0 //Meter factor for calibration

#define PRESSUREREADDELAY       20L // wait 20 ms between reads

int   rawSensorValue = 0;          //Measured differential pressure sensor raw value in bits.

long randomNumber;               // Use for testing

float pressureSensorVoltage;     //Measured differential pressure sensor voltage
float delP;                      //Measured differential pressure sensor Flow Delta Pressure in Kpa
float instantFlowValue;          //Instantaneous Flow
float averageStoredFlowValues;   //Stored Instantaneous flow Values for averaging
float averageInstantFlowValue;          //Average Flow Rate
float inAverageFlowRate;         //Inhale flow rate in Liters/min
float exAverageFlowRate;         //exhale flow rate in Liters/min
float inLitersPerCycle;          //Liters Per Inhale Cycle
float exLitersPerCycle;          //Liters Per exhale Cycle
bool  exhale;                    //Boolean to indicate if on the exhale cycle
bool  lastMode;                  //Boolean for switching from Inhale to exhale
long  cTime;                      //time of last computations for the integrator
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
  Purpose: Calculate the change in pressure, delP, from the sensor-generated voltage

  Argument list:
    void

  Return value:
    float         the voltage value expressed as kPa

  CAUTION:
*****/
float ReadFlowPressureSensor() 
{
#ifdef DEBUG                        // Fake a test value

  randomNumber          = random(0, 1023);                                        // Simulate a voltage reading  
  pressureSensorVoltage = ((float) map(randomNumber, 0, 1023, 0, 500)) / 100.0;   //Make it look like a voltage between 0-5V
  delP                  =  (5.0 * pressureSensorVoltage / VSOURCE) - 2.5;         //Convert volts to kPa  Return delP;
 
  Serial.print("ReadFlowPressureSensor    ");
  Serial.print("randomNumber = ");
  Serial.print(randomNumber);
  Serial.print("   pressureSensorVoltage = ");
  Serial.print(pressureSensorVoltage);
  Serial.print("    delP = ");
  Serial.println(delP);  

  return delP;
#endif
                                    // Do the real thing here
                  
  rawSensorValue        = analogRead(PRESSURESENSORPIN);                          //Raw digital input from pressure sensor
  pressureSensorVoltage = ((float) map(rawSensorValue, 0, 1023, 0, 5)) / 100.0;   //Convert raw bits read to volts
  delP                  = (5.0 * pressureSensorVoltage / VSOURCE) - 2.5;          //Convert volts to kPa  Return delP;
  return delP;
}


/*****
  Purpose: Calculate the instantaneous flow using the flow pressure

  Argument list:
    void

  Return value:
    float       instantaneous flow rate

  CAUTION:
*****/
float CalculateInstantFlow() 
{
  float correction;                 // Used in calculation to avoid square root of negative number
  if (delP < 0)
    correction = -1.0;
  else
    correction = 1.0;
  instantFlowValue = MF * PTFCC * sqrt(delP * correction);
  instantFlowValue *= correction;

#ifdef DEBUG1
  Serial.print("CalculateInstantFlow");
  Serial.print("    delP = ");
  Serial.print(delP);  
  Serial.print("    instantFlowValue = ");
  Serial.println(instantFlowValue);    
#endif

  return instantFlowValue;
}

/*****
  Purpose: Calculate the change in pressure, delP, from the sensor-generated voltage

  Argument list:
    void

  Return value:
    void

  CAUTION:
*****/
void CalculateAverageFlowRate() 
{
  averageInstantFlowValue = 0.0;
  averageStoredFlowValues = 0.0;
  for (j = 0; j < AVERAGEBINNUMBER; j++) {
    averageStoredFlowValues += ReadFlowPressureSensor();
    averageInstantFlowValue += CalculateInstantFlow();
    MyDelay(PRESSUREREADDELAY);    
  }
  averageStoredFlowValues /= (float) AVERAGEBINNUMBER;
  averageInstantFlowValue /= (float) AVERAGEBINNUMBER;
  exhale = true;
  if (averageInstantFlowValue > 0.0) {
    exhale = false;
  }
#ifdef DEBUG
  Serial.print("CalculateAverageFlowRate");
  Serial.print("exhale = ");
  Serial.print(exhale);  
  Serial.print("     averageStoredFlowValues = ");
  Serial.print(averageStoredFlowValues);  
  Serial.print("   averageInstantFlowValue = ");
  Serial.println(averageInstantFlowValue);      
#endif
}

/*****
  Purpose: Calculate the flow rate in liters

  Argument list:
    void

  Return value:
    void

  CAUTION:
*****/
void CalculateTotalLiters()
{
  float tempC, nowTime, dTime;

  tempC = 0.0;
  nowTime = (float) millis();
  dTime = nowTime - cTime;    //Time in milliseconds since the last integration step
  if (dTime <= 0.0) {
    dTime = 1.0;
  }
  cTime = nowTime;
  tempC = averageInstantFlowValue * dTime / 1000000.0;  // this is flowrate in Liters per min * ms /(1000000ms/min)
  if (exhale) {
    exLitersPerCycle = exLitersPerCycle - tempC;
  }
  else {
    inLitersPerCycle = inLitersPerCycle + tempC;
  }
#ifdef DEBUG
  Serial.print("CalculateTotalLiters");
  Serial.print("    exLitersPerCycle = ");
  Serial.print(exLitersPerCycle);
  Serial.print("    inLitersPerCycle = ");
  Serial.print(inLitersPerCycle);
  Serial.print("    dTime = ");
  Serial.print(dTime);
  Serial.print("    tempC = ");
  Serial.println(tempC);           
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
  exhale            = false;
  lastMode          = false;
  averageInstantFlowValue  = 0.0;
  inAverageFlowRate = 0.0;
  exAverageFlowRate = 0.0;
  
  cTime = millis();
}


void loop()
{
  CalculateAverageFlowRate();
  if (exhale != lastMode) {     //means we switched from exhale to Inhale or vice versa so start the counters over
    if (exhale) {
      inAverageFlowRate = 0.0;  //Zero out the Inflow because we are in exhale mode
      exLitersPerCycle  = 0.0;  //Start the exhale flow integrator over
    } else {
      exAverageFlowRate = 0.0;  //Zero out the Exflow because we are in Inhale mode
      inLitersPerCycle  = 0.0;   //Start the Inhale flow integrator over
    }
  }
  if (exhale) {
    exAverageFlowRate = averageInstantFlowValue;  //Set the exhale average flow to the Average Flow because we are exhaling
  } else {
    inAverageFlowRate = averageInstantFlowValue;  //Set the Inhale average flow to the average flow because we are inhaling
  }
  CalculateTotalLiters();  //calculate total liters either inhaled or exhaled this cycle by integrating...

  lastMode = exhale;          //Last mode was exhale? True or false


  // Here you should print out the inAverageFlowRate, exAverageFlowRate, inLitersPerCycle, and exLitersPerCycle to the LCD
#ifdef DEBUG
  Serial.println("\nMAIN LOOP");
  Serial.print("lastMode = ");
  Serial.print(lastMode);  
  Serial.print("   inAverageFlowRate = ");
  Serial.print(inAverageFlowRate);
  Serial.print("   exAverageFlowRate = ");
  Serial.print(exAverageFlowRate);
  Serial.print("   inLitersPerCycle  = ");
  Serial.print(inLitersPerCycle);
  Serial.print("   exLitersPerCycle  = ");
  Serial.println(exLitersPerCycle);
  #endif  
}
