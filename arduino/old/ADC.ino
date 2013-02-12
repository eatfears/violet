/****************************************************
 *ADC interrupt vector and ADC interface
 ****************************************************/
// We are using an oversampling and averaging method to increase the ADC resolution
// The theorical ADC resolution is now 11.7 bits. Now we store the ADC readings in float format

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
  float k = 1;
  if ( select > 4) k = 1.52;

  if (SENSOR_SIGN[select]<0)
    return sensor_postscale[select]*(AN_OFFSET[select]-k*AN[select]);
  else
    return sensor_postscale[select]*(k*AN[select]-AN_OFFSET[select]); //returns scalled and offseted value
}

//Activating the ADC interrupts. 
void Analog_Init(void)
{
  ADCSRA|= (1<<ADIE)|(1<<ADEN);
  ADCSRA|= (1<<ADSC);
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




