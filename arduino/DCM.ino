/****************************************************
 *DCM algorithm to compute rotation matrix
 ****************************************************/
#define GRAVITY 263 //this equivalent to 1G in the raw data coming from the accelerometer

float Update_Matrix[3][3]={
  0,1,2,3,4,5,6,7,8}; //Gyros here
float Temporary_Matrix[3][3]={
  0,0,0,0,0,0,0,0,0};

float errorRollPitch[3]={
  0,0,0};

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


void Normalize(void)
{
  float error=0;
  float temporary[3][3];
  float renorm=0;

  error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19

  Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
  Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19

  Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
  Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19

  Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20

  renorm= .5 *(3 - Vector_Dot_Product(&temporary[0][0],&temporary[0][0])); //eq.21
  Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);

  renorm= .5 *(3 - Vector_Dot_Product(&temporary[1][0],&temporary[1][0])); //eq.21
  Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);

  renorm= .5 *(3 - Vector_Dot_Product(&temporary[2][0],&temporary[2][0])); //eq.21
  Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
}

/**************************************************/
void Drift_correction(void)
{
  //Compensation the Roll, Pitch and Yaw drift. 
  float errorCourse;
  static float Scaled_Omega_P[3];
  static float Scaled_Omega_I[3];
  float Accel_magnitude;
  float Accel_weight;

  //*****Roll and Pitch***************

  // Calculate the magnitude of the accelerometer vector
  Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
  Accel_magnitude = Accel_magnitude / GRAVITY; // Scale to gravity.
  // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
  Accel_weight = constrain(1 - 2*abs(1 - Accel_magnitude),0,1);

  Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
  Vector_Scale(&Omega_P[0],&errorRollPitch[0],Kp_ROLLPITCH*Accel_weight);

  Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],G_Dt*Ki_ROLLPITCH*Accel_weight);
  Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);

  //*****YAW***************
  /*
  // We make the gyro YAW drift correction based on compass magnetic heading
   mag_heading_x = cos(MAG_Heading);
   mag_heading_y = sin(MAG_Heading);
   errorCourse= (DCM_Matrix[0][0]*mag_heading_y) - (DCM_Matrix[1][0]*mag_heading_x);  //Calculating YAW error
   Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
   
   Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);
   Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.
   
   Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);
   Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I
   */

}

/**************************************************/
void Matrix_update(void)
{
  Gyro_Vector[0] = Gyro_Scaled_Y_4_5(read_adc(3)); //gyro x roll
  Gyro_Vector[1] = Gyro_Scaled_X_4_5(read_adc(1)); //gyro y pitch
  Gyro_Vector[2] = 0;//*Gyro_Scaled_Z(read_adc(2)); //gyro Z yaw

  if(abs(Gyro_Vector[0]) > 2.35) Gyro_Vector[0] = Gyro_Scaled_Y(read_adc(2));
  if(abs(Gyro_Vector[1]) > 2.55) Gyro_Vector[1] = Gyro_Scaled_X(read_adc(0));

  Accel_Vector[0] = read_adc(5); // acc x
  Accel_Vector[1] = read_adc(6); // acc y
  Accel_Vector[2] = read_adc(7); // acc z

  Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);//adding integrator
  Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]);//adding proportional

  Update_Matrix[0][0]=0;
  Update_Matrix[0][1]=-G_Dt*Omega_Vector[2];//-z
  Update_Matrix[0][2]=G_Dt*Omega_Vector[1];//y
  Update_Matrix[1][0]=G_Dt*Omega_Vector[2];//z
  Update_Matrix[1][1]=0;
  Update_Matrix[1][2]=-G_Dt*Omega_Vector[0];//-x
  Update_Matrix[2][0]=-G_Dt*Omega_Vector[1];//-y
  Update_Matrix[2][1]=G_Dt*Omega_Vector[0];//x
  Update_Matrix[2][2]=0;

  Matrix_Multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c

  for(int x=0; x<3; x++)  //Matrix Addition (update)
  {
    for(int y=0; y<3; y++)
    {
      DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
    } 
  }
}

void Euler_angles(void)
{
  pitch = asin(-DCM_Matrix[2][0]);
  roll = atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]);
  yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
}







