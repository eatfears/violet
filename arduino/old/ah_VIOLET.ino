 /*
Channels:
 * 1 - ROLL
 * 2 - Tortle
 * 3 - PITCH
 * 4 - YAW
 * 5 - Engine start
 * 6 - Mode
 
 Rotors:
 * 0 - Front
 * 1 - Back
 * 2 - Right
 * 3 - Left
 */
#include <inttypes.h>
#include <math.h>
#include <Wire.h>   // For magnetometer readings
#include <aJSON.h>

#define MIN_POWER 1050 //ESC's were calibrated with RC transmitter
#define MAX_POWER 1800 //find this with ..\af_Radio\ac_Wide

#define PWM_PERIOD_4 5000 //usec. This is PWM period divided by 4 
//find PWM frequency with ..\af_Radio\ab_Freq

#define NUM_RC_CHANNELS 6
#define NUM_ESC 4


// Channel values
int ch1, ch2, ch3, ch4, ch_aux, ch_aux2;

int Neutro[NUM_RC_CHANNELS+1];

int ESC_check_pin[NUM_ESC] = { //53
  -1, -1, -1, -1}; //should be +5V, when ESC started

int PWM_OUT_PORT[NUM_ESC] = {
  //  49, 43, -1, -1}; // out ports to ESC
  49, 43, 47, 45}; // out ports to ESC

int RC_state = 1;

int Init_complete = 0;
char key;

int PWM_power[NUM_ESC]; // write value here (=
//should be from MIN_POWER to MAX_POWER. 0 if no signal

//------------------------Stabilization values----------------


//Sensor: GYROX, GYROX45, GYROY, GYROY45, TEMP, ACCELX, ACCELY, ACCELZ
uint8_t sensors[8]={
  0,1,2,3,4,14,15,13};  // For Hardware v1 flat
int SENSOR_SIGN[8]={
  1,1,1,1,1,1,1,1}; //{1,-1,-1,-1,1,-1}


#define acc_offset_x 515//520 
#define acc_offset_y 546//545
#define acc_offset_z 527//524       // We need to rotate the IMU exactly 90º to take this value  
#define gyro_offset_roll 271.01//y  
#define gyro_offset_pitch 276.00 //x
#define gyro_offset_roll_4_5 257.36
#define gyro_offset_pitch_4_5 277.72
#define gyro_offset_yaw 380

#define GRAVITY 263 //this equivalent to 1G in the raw data coming from the accelerometer
float sensor_postscale[8]={ 
  1, 1, 1, 1, 1, 1, 1.027, 1.04};

#define Accel_Scale(x) x*(GRAVITY/9.81)//Scaling the raw data of the accel to actual acceleration in meters for seconds square

#define ToRad(x) (x*0.01745329252)  // *pi/180
#define ToDeg(x) (x*57.2957795131)  // *180/pi

// LPR530 & LY530 Sensitivity (from datasheet) => 2.0mV/º/s, => 5/(0.002*1024)
// Tested values : 0.96,0.96,0.94
#define Gyro_Gain_X 2.4414 //X axis Gyro gain
#define Gyro_Gain_Y 2.4414 //Y axis Gyro gain
#define Gyro_Gain_X_4_5 0.53657 //X axis Gyro gain
#define Gyro_Gain_Y_4_5 0.53657 //Y axis Gyro gain
#define Gyro_Gain_Z 0.94 //Z axis Gyro gain
#define Gyro_Scaled_X(x) x*ToRad(Gyro_Gain_X) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) x*ToRad(Gyro_Gain_Y) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_X_4_5(x) x*ToRad(Gyro_Gain_X_4_5) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y_4_5(x) x*ToRad(Gyro_Gain_Y_4_5) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) x*ToRad(Gyro_Gain_Z) //Return the scaled ADC raw data of the gyro in radians for second

// QuadCopter Attitude control PID GAINS
#define KP_QUAD_ROLL 2.2
#define KD_QUAD_ROLL 0.36 
#define KI_QUAD_ROLL 0.4 
#define KP_QUAD_PITCH 2.2 //1.6 //1.75 //1.8 // 2.2   //1.75
#define KD_QUAD_PITCH 0.36 //0.4 //0.42 // 0.54  //0.45
#define KI_QUAD_PITCH 0.4 //0.42 // 0.45  //0.5
#define KP_QUAD_YAW 1.0 // 3.5 // 4.6  //3.2 //2.6
#define KD_QUAD_YAW 1.0 // 0.7  //0.8 //0.4
#define KI_QUAD_YAW 0.15 // 0.2  //0.15

#define KD_QUAD_COMMAND_PART 13.0   // for special KD implementation (in two parts). Higher values makes the quadcopter more responsive to user inputs

#define Kp_ROLLPITCH 0.008  //0.0125 //0.010 // Pitch&Roll Proportional Gain
#define Ki_ROLLPITCH 0.000010 // Pitch&Roll Integrator Gain
#define Kp_YAW 10.0 // Yaw Porportional Gain  
#define Ki_YAW 0.00005 // Yaw Integrator Gain

/*For debugging purposes*/
#define OUTPUTMODE 1  //If value = 1 will print the corrected data, 0 will print uncorrected data of the gyros (with drift), 2 Accel only data

float DCM_Matrix[3][3]={
  1,0,0,0,1,0,0,0,1}; 
float Update_Matrix[3][3]={
  0,1,2,3,4,5,6,7,8}; //Gyros here
float Temporary_Matrix[3][3]={
  0,0,0,0,0,0,0,0,0};

float AN[8]; //array that store the 6 ADC filtered data
float AN_OFFSET[8]; //Array that stores the Offset of the gyros

float G_Dt=0.02;    // Integration time for the gyros (DCM algorithm)

float Accel_Vector[3]={
  0,0,0}; //Store the acceleration in a vector
float Accel_Vector_unfiltered[3]={
  0,0,0}; //Store the acceleration in a vector
float Accel_magnitude;
float Accel_weight;
float Gyro_Vector[3]={
  0,0,0};//Store the gyros rutn rate in a vector
float Omega_Vector[3]={
  0,0,0}; //Corrected Gyro_Vector data
float Omega_P[3]={
  0,0,0};//Omega Proportional correction
float Omega_I[3]={
  0,0,0};//Omega Integrator
float Omega[3]={
  0,0,0};

float errorRollPitch[3]={
  0,0,0};
float errorYaw[3]={
  0,0,0};
float errorCourse=0;
float COGX=0; //Course overground X axis
float COGY=1;


// ADC variables
volatile uint8_t MuxSel=0;
volatile uint8_t analog_reference = DEFAULT;
volatile uint16_t analog_buffer[16];
volatile uint8_t analog_count[16];
int an_count;

float roll=0;
float pitch=0;
float yaw=0;

//Magnetometer variables
int magnetom_x;
int magnetom_y;
int magnetom_z;
float MAG_Heading = 0;
float MAG_Heading_to = 0;
float mag_heading_x;
float mag_heading_y;

int num_iter;

unsigned int counter=0;

long timer=0; //general porpuse timer 
long timer_old;



// Attitude control variables
float command_rx_roll=0;        // comandos recibidos rx
float command_rx_roll_old =0;
float command_rx_roll_diff=0;
float command_rx_pitch=0;
float command_rx_pitch_old=0;
float command_rx_pitch_diff=0;
float command_rx_yaw=0;
float command_rx_yaw_diff=0;
int control_roll=0;           // resultados del control
int control_pitch=0;
int control_yaw=0;
float K_aux=0;

// Attitude control
float roll_I=0;
float roll_D=0;
float err_roll=0;
float pitch_I=0;
float pitch_D=0;
float err_pitch=0;
float yaw_I=0;
float yaw_D=0;
float err_yaw=0;
/* ************************************************************ */
// ROLL, PITCH and YAW PID controls...
void Attitude_control()
{
  float err = (ch_aux2 - 1500)/100.0;
  //Serial.println (err);
  // ROLL CONTROL    
  if (1)//AP_mode==2)        // Normal Mode => Stabilization mode
    err_roll = command_rx_roll - 0.36 - ToDeg(roll); //2.35

  err_roll = constrain(err_roll,-30,30);  // to limit max roll command...

  roll_I += err_roll*G_Dt;
  roll_I = constrain(roll_I,-50,50);
  // D term implementation => two parts: gyro part and command part
  // To have a better (faster) response we can use the Gyro reading directly for the Derivative term...
  // Omega[] is the raw gyro reading plus Omega_I, so it´s bias corrected
  // We also add a part that takes into account the command from user (stick) to make the system more responsive to user inputs
  roll_D = command_rx_roll_diff*KD_QUAD_COMMAND_PART - ToDeg(Omega[0]);  // Take into account Angular velocity of the stick (command)

  // PID control
  control_roll = KP_QUAD_ROLL*err_roll + KD_QUAD_ROLL*roll_D + KI_QUAD_ROLL*roll_I; 

  // PITCH CONTROL
  if (1)//AP_mode==2)        // Normal mode => Stabilization mode
    err_pitch = command_rx_pitch - 0.01 - ToDeg(pitch); //2.91

  err_pitch = constrain(err_pitch,-30,30);  // to limit max pitch command...

  pitch_I += err_pitch*G_Dt;
  pitch_I = constrain(pitch_I,-50,50);
  // D term
  pitch_D = command_rx_pitch_diff*KD_QUAD_COMMAND_PART - ToDeg(Omega[1]);

  // PID control
  control_pitch = KP_QUAD_PITCH*err_pitch + KD_QUAD_PITCH*pitch_D + KI_QUAD_PITCH*pitch_I; 

  // YAW CONTROL

  err_yaw = command_rx_yaw - ToDeg(yaw) + ToDeg(MAG_Heading_to);
  while (err_yaw >= 180)    // Normalize to -180,180
      err_yaw -= 360;
  while(err_yaw < -180)
    err_yaw += 360;
  

  err_yaw = constrain(err_yaw,-60,60);  // to limit max yaw command...

  yaw_I += err_yaw*G_Dt;
  yaw_I = constrain(yaw_I,-50,50);
  yaw_D = command_rx_yaw_diff*KD_QUAD_COMMAND_PART - ToDeg(Omega[2]);


  //if(ch_aux2 > 1500) { roll_I = 0; pitch_I = 0; yaw_I = 0; MAG_Heading_to =  MAG_Heading;}
  //if(ch_aux2 > 1700) { }
  // PID control
  control_yaw = KP_QUAD_YAW*err_yaw + KD_QUAD_YAW*yaw_D + KI_QUAD_YAW*yaw_I;
}

int channel_filter(int ch, int ch_old)
{
  if (ch_old==0)
    return(ch);
  else
    return((ch+ch_old)>>1);     // small filtering (average filter)
}

/* ******** SETUP **************************************************** */
void setup() 
{
  int i, j;
  Serial.begin(115200); 
  Serial.println("------------------------------");
  Serial.println("|   Quadcopter VIOLET 2010   |");
  Serial.println("------------------------------");
  int in_status = Init_and_check();

  for (int i = 0; i < NUM_ESC; i++ )
  {
    PWM_power[i] = MIN_POWER;
    ServoPowerSet(i, MIN_POWER);
  }

  PWM_init();
  RC_read_init();

  delay(50);
  if (!RC_state) 
  {
    if (in_status != -1)
      in_status = -2;
    Serial.println("Warning!\nNo RC signal.\n");
  }

  // Take neutral radio values...
  if (!RC_state)
  {
    // Roll, Pitch and Throttle have fixed neutral values (the user can trim the radio)
    Neutro[1] = 1500;
    Neutro[2] = MIN_POWER;
    Neutro[3] = 1500;
    Neutro[4] = 1500;
    Neutro[5] = 1500;
    Neutro[6] = 1500;
  }
  else
  {  
    for (j=1; j<=6; j++)
      Neutro[j] = RxGetChannelPulseWidth(j);
    int times = 150;
    for (i=0; i<times; i++)
    {
      for (j=1; j<=6; j++)
        Neutro[j] = (Neutro[j]*0.8 + RxGetChannelPulseWidth(j)*0.2);
      delay(25);
    }
  }

  Serial.print("Rx values: ");
  for(i=1;i<=6;i++)
  { 
    Serial.print(" ");
    Serial.print(Neutro[i]);
    Serial.print(",");
  }
  Serial.println();


  Analog_Reference(DEFAULT);
  Analog_Init();

  I2C_Init();
  delay(100);
  Compass_Init();

  Read_adc_raw();

  AN_OFFSET[0] = gyro_offset_roll;
  AN_OFFSET[1] = gyro_offset_roll_4_5;
  AN_OFFSET[2] = gyro_offset_pitch;
  AN_OFFSET[3] = gyro_offset_pitch_4_5;
  AN_OFFSET[4] = 0;
  AN_OFFSET[5] = acc_offset_x;
  AN_OFFSET[6] = acc_offset_y;
  AN_OFFSET[7] = acc_offset_z;


  // Take the gyro offset values
  if(1)
  { 
    for(i=0;i<300;i++)
    {
      Read_adc_raw();
      for(j=0; j<4; j++)   // Read initial ADC values for offset.
        AN_OFFSET[j]=AN_OFFSET[j]*0.8 + AN[j]*0.2;
      delay(20);
    }
  }

  Serial.println("AN_OFFSETs:");
  for(i=0;i<8;i++)
  { 
    Serial.print(i);
    Serial.print(": ");
    Serial.println(AN_OFFSET[i]);
  }

  Read_Compass();
  MAG_Heading_to =  MAG_Heading;
  

  switch (in_status)
  { 
  case -1:
    Serial.println("Crytical error\nHalt.\n\n");
    PWM_interrupt_stop();
    for(;;);
    break;

  case -2:
    Serial.println("WARNING");
    break;

  case 0:
    Serial.println("Ok");
    break;
  }

  Serial.println("All systems go!\n");
  Init_complete = 1;

  Read_adc_raw();   // Start ADC readings...
  timer = millis();
  delay(20);
}

void loop() 
{
  int aux;
  float aux_float;

  if((millis()-timer)>=14)   // 14ms => 70 Hz loop rate 
  {
    counter++;
    //loop_counter++;
    //Serial.println(millis());
    timer_old = timer;
    timer = millis();
    G_Dt = (timer-timer_old)/1000.0;      // Real time of loop run 
    num_iter++;

    //key = check_for_key();
    Read_adc_raw();

    if (counter > 7)  // Read compass data at 10Hz... (7 loop runs)
    {
      counter=0;
      Read_Compass();    // Read I2C magnetometer
      // Compass_Heading(); // Calculate magnetic heading  
    } 

    Matrix_update(); 
    Normalize();
    Drift_correction();
    Euler_angles();


    // Telemetry data...
    if (0)
    {
      aux_float = ToDeg(roll);
      Serial.print(aux_float);
      Serial.print(",");
      aux_float = (ToDeg(pitch));
      Serial.print(aux_float);
      Serial.print(",");
      aux_float = ToDeg(yaw);
      Serial.print(aux_float);
      Serial.println();
    }
    if (0)
    {
      Serial.println(MAG_Heading);
    }
    if (0)
    {
      Serial.print(read_adc(7));
      Serial.println();
    }
    if (0)
    {
      Serial.println(control_yaw);
    }

    ch1 = channel_filter(RxGetChannelPulseWidth(1),ch1);
    ch2 = channel_filter(RxGetChannelPulseWidth(2),ch2);
    ch3 = channel_filter(RxGetChannelPulseWidth(3),ch3);
    ch4 = channel_filter(RxGetChannelPulseWidth(4),ch4);
    ch_aux = channel_filter(RxGetChannelPulseWidth(5),ch_aux);
    ch_aux2 = channel_filter(RxGetChannelPulseWidth(6),ch_aux2);

    // Commands from radio Rx... 
    // Stick position defines the desired angle in roll, pitch and yaw
    command_rx_roll_old = command_rx_roll;
    command_rx_roll = 0*(ch1-Neutro[1])/13.0;
    if (abs(ch1-Neutro[1]) <= 30) command_rx_roll = 0;
    command_rx_roll_diff = command_rx_roll-command_rx_roll_old;
    
    
    command_rx_pitch_old = command_rx_pitch;
    command_rx_pitch = 0*(ch3-Neutro[3])/13.0;
    if (abs(ch3-Neutro[3]) <= 30) command_rx_pitch = 0;
    command_rx_pitch_diff = command_rx_pitch-command_rx_pitch_old;
    //Serial.println(command_rx_pitch);
    
    aux_float = 0*(ch4-Neutro[4])/400.0;
    
    //Serial.println(ch4-Neutro[4]);
    if (abs(ch4-Neutro[4]) <= 25) aux_float = 0;
    
    command_rx_yaw += aux_float;
    command_rx_yaw_diff = aux_float;
    if (command_rx_yaw >= 180)         // Normalize yaw to -180,180 degrees
      command_rx_yaw -= 360.0;
    else if (command_rx_yaw < -180)
      command_rx_yaw += 360.0;


    Attitude_control();

    //Serial.println(control_pitch);
    //control_pitch = 0;
    //control_roll = 0;
    //control_yaw = 0;

    //Serial.println(RC_Time[2]);


float nul = 1125.0;
    PWM_power[0] = (ch2-nul)/2.0+nul - control_pitch + control_yaw;
    PWM_power[1] = (ch2-nul)/2.0+nul + control_pitch + control_yaw;
    PWM_power[2] = (ch2-nul)/2.0+nul - control_roll - control_yaw;
    PWM_power[3] = (ch2-nul)/2.0+nul + control_roll - control_yaw;

    //Serial.println(ch2);
    
    for (int i = 0; i < NUM_ESC; i++ ) PWM_power[i] = constrain(PWM_power[i], MIN_POWER, MAX_POWER);

    if ((!RC_state)||(ch_aux < 1300)||(ch2 < 1150))
    {
    
      for (int i = 0; i < NUM_ESC; i++ ) PWM_power[i] = MIN_POWER;
      roll_I = 0; pitch_I = 0; yaw_I = 0;  MAG_Heading_to =  MAG_Heading;
    }
    for (int i = 0; i < NUM_ESC; i++ ) ServoPowerSet(i, PWM_power[i]);
    //Serial.println(millis());Serial.println();
  }

}

int Init_and_check()
{
  int state = 0;

  for (int i = 0; i <= 2; i++)
    if (RC_IN_PORT[i] >= 0) 
      pinMode(RC_IN_PORT[i], INPUT); 

  for (int i = 0; i < NUM_ESC; i++)
    if (PWM_OUT_PORT[i] >= 0)
    {
      pinMode(PWM_OUT_PORT[i], OUTPUT);
      if (ESC_check_pin[i] >= 0)
        pinMode(ESC_check_pin[i], INPUT); 
      PWM_power[i] = MIN_POWER;
    }


  for (int i = 0; i < NUM_ESC; i++)
    if (PWM_OUT_PORT[i] >= 0)
    {
      if (ESC_check_pin[i] >= 0)
      {
        if (!digitalRead(ESC_check_pin[i]))
        {
          Serial.println("Error!");
          Serial.print("ESC ");
          Serial.print(i);
          Serial.println(" fail\n");
          state = -1;
        }
      }
      else 
      {
        if (state != -1)
          state = -2;
        Serial.println("Warning!");
        Serial.print("ESC ");
        Serial.print(i);
        Serial.println(" is not checked");
        Serial.println("Please ensure, that ESC is operational\n");
      }
    }

  return state;
}

char check_for_key()
{
  char incomingByte;
  incomingByte = 0;

  if (Serial.available() > 0)
    incomingByte = Serial.read();
  return incomingByte;
}















