/*
  Channels:
   1 - ROLL
   2 - Tortle
   3 - PITCH
   4 - YAW
   5 - Engine start
   6 - Mode

  Rotors:
   0 - Front
   1 - Back
   2 - Right
   3 - Left
*/
#include <inttypes.h>
#include <math.h>
#include <Wire.h>   // For magnetometer readings

#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi

float MAG_Heading = 0;

float DCM_Matrix[3][3] = {
  1, 0, 0, 0, 1, 0, 0, 0, 1
};

float Accel_Vector[3] = {
  0, 0, 0
}; //Store the acceleration in a vector

float AN[8]; //array that store the 6 ADC filtered data
float AN_OFFSET[8]; //Array that stores the Offset of the gyros

float roll = 0;
float pitch = 0;
float yaw = 0;

float G_Dt = 0.02;  // Integration time for the gyros (DCM algorithm)

unsigned int counter = 0;
long timer = 0; //general porpuse timer
long timer_old;

void setup()
{
  Serial.begin(115200);
  Analog_Reference(DEFAULT);
  Analog_Init();

  I2C_Init();
  delay(100);
  Compass_Init();

  Take_Gyro_Offset();

  Read_Compass();

  RC_read_init();
  PWM_init();
}

extern int RC_state;
void loop()
{
  if ((millis() - timer) >= 14) // 14ms => 70 Hz loop rate
  {
    counter++;
    timer_old = timer;
    timer = millis();
    G_Dt = (timer - timer_old) / 1000.0;  // Real time of loop run

    Read_adc_raw();

    if (counter > 7)  // Read compass data at 10Hz... (7 loop runs)
    {
      counter = 0;
      Read_Compass();    // Read I2C magnetometer
      // Compass_Heading(); // Calculate magnetic heading
    }

    Matrix_update();
    Normalize();
    Drift_correction();
    Euler_angles();
  }

  static long serial_timer = millis();
  if ((millis() - serial_timer) >= 50) // 50ms => 20 Hz loop rate
  {
    serial_timer = millis();

    for (int i = 0; i < 6; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(RxGetChannelPulseWidth(i));
      Serial.print(" ");
    }
    Serial.print("RC: ");
    Serial.print(RC_state);
    Serial.println("");
    //    Serial.println("hello");
    //    Serial.println(pitch);
    //    Serial.println(roll);
    //    Serial.println(yaw);
  }
}
