/****************************************************
 *ADC interrupt vector and ADC interface
 ****************************************************/
// We are using an oversampling and averaging method to increase the ADC resolution
// The theorical ADC resolution is now 11.7 bits. Now we store the ADC readings in float format

#define acc_offset_x 515//520 
#define acc_offset_y 546//545
#define acc_offset_z 527//524       // We need to rotate the IMU exactly 90º to take this value  
#define gyro_offset_roll 271.01//y  
#define gyro_offset_pitch 276.00 //x
#define gyro_offset_roll_4_5 257.36
#define gyro_offset_pitch_4_5 277.72
#define gyro_offset_yaw 380

//Sensor: GYROX, GYROX45, GYROY, GYROY45, TEMP, ACCELX, ACCELY, ACCELZ
uint8_t sensors[8]={
  0,1,2,3,4,14,15,13};  // For Hardware v1 flat
int SENSOR_SIGN[8]={
  -1,-1,1,1,1,-1,1,1}; //{1,-1,-1,-1,1,-1}

// ADC variables
volatile uint8_t MuxSel=0;
volatile uint8_t analog_reference = DEFAULT;
volatile uint16_t analog_buffer[16];
volatile uint8_t analog_count[16];

void Read_adc_raw(void)
{
  int i;
  int temp1;
  int temp2;

  // ADC readings...
  for (i=0; i<8; i++)
  {
    temp1= analog_buffer[sensors[i]];   // sensors[] maps sensors to correct order 
    temp2= analog_count[sensors[i]];

    if (temp1 != analog_buffer[sensors[i]])  // Check if there was an ADC interrupt during readings...
    {
      temp1= analog_buffer[sensors[i]];      // Take a new reading
      temp2= analog_count[sensors[i]];
    }
    if (temp2!=0)
      AN[i] = float(temp1)/float(temp2);
    //Serial.print(i);Serial.print(" "); Serial.println(temp2); //how many readings we accumulate
  }

  // Initialization for the next readings...
  for (int i=0; i<8; i++){
    analog_buffer[sensors[i]]=0;
    analog_count[sensors[i]]=0;
    if (analog_buffer[sensors[i]]!=0) // Check if there was an ADC interrupt during initialization...
    {
      analog_buffer[sensors[i]]=0;    // We do it again...
      analog_count[sensors[i]]=0;  
    }
  }
}

float read_adc(int select)
{
  float k = 1.0;
  if (select > 4) k = 1.515;

  if (SENSOR_SIGN[select]<0)
    return (AN_OFFSET[select]-k*AN[select]);
  else
    return (k*AN[select]-AN_OFFSET[select]); //returns scalled and offseted value
}

//Activating the ADC interrupts. 
void Analog_Init(void)
{
  ADCSRA|= (1<<ADIE)|(1<<ADEN);
  ADCSRA|= (1<<ADSC);

  AN_OFFSET[0] = gyro_offset_roll;
  AN_OFFSET[1] = gyro_offset_roll_4_5;
  AN_OFFSET[2] = gyro_offset_pitch;
  AN_OFFSET[3] = gyro_offset_pitch_4_5;
  AN_OFFSET[4] = 0;
  AN_OFFSET[5] = acc_offset_x;
  AN_OFFSET[6] = acc_offset_y;
  AN_OFFSET[7] = acc_offset_z;

}

//
void Analog_Reference(uint8_t mode)
{
  analog_reference = mode;
}

//ADC interrupt vector, this piece of code
//is executed everytime a convertion is finished. 
ISR(ADC_vect)
{
  volatile uint8_t low, high;
  low = ADCL;
  high = ADCH;
  analog_buffer[sensors[MuxSel]] += (high << 8) | low;   // cumulate analog values
  analog_count[sensors[MuxSel]]++;
  MuxSel++;
  if(MuxSel > 7) MuxSel=0; //select next port
  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((sensors[MuxSel] >> 3) & 0x01) << MUX5);
  ADMUX = (analog_reference << 6) | (sensors[MuxSel] & 0x07);//prepare registers

  // start the new conversion
  ADCSRA|= (1<<ADSC);
}

void Take_Gyro_Offset()
{
  Read_adc_raw();
  for(int i=0;i<200;i++)
  {
    Read_adc_raw();
    for(int j=0; j<4; j++)   // Read initial ADC values for offset.
      AN_OFFSET[j]=AN_OFFSET[j]*0.8 + AN[j]*0.2;
    delay(20);
  }
}

