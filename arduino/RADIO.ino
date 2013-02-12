/****************************************************
 *Radio and PWM driver interupts
 ****************************************************/
//1 1112 - 1520 - 1908
//2 1112 - 1508 - 1888
//3 1104 - 1492 - 1884


#define PWM_PERIOD_4 5000 //usec. This is PWM period divided by 4 
//find PWM frequency with ..\af_Radio\ab_Freq

#define NUM_RC_CHANNELS 6
#define NUM_ESC 4

int RC_IN_PORT[2] = {2, 3}; //in ports from RC

int PWM_OUT_PORT[NUM_ESC] = {
  //  49, 43, -1, -1}; // out ports to ESC
  49, 43, 47, 45}; // out ports to ESC


int PWM_power_ready[NUM_ESC];
int PWM_power_s[NUM_ESC];
unsigned long RC_Old[NUM_RC_CHANNELS + 1] = {
  0,0,0,0,0,0,0};
int ESC_stage = 0;
int RC_select = 1;
unsigned long RC_check_time;
int RC_state = 1;


unsigned long RC_Time[NUM_RC_CHANNELS + 1]= { 
  0, 0, 0, 0, 0, 0, 0};  //input value from RC
//Read this to know the RC transmitter values
// if 6 channels. 1-6. RC_Time[0] is not used

void PWM_init()
{
  TCCR1A = 0x00;	     // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,PWM10=0 => PWM 
  TCCR1B = 0x42;	     // TCNT1 preescaler/8 (16Mhz => 0.5useg, 8 Mhz => 1useg), Rising edge
  TIMSK1 = _BV(ICIE1)|_BV (TOIE1);   // Enable interrupts : Timer1 Capture and Timer1 Overflow
}

void PWM_interrupt_stop()
{
  TCCR1A = 0x01;
  TIMSK1 = 0;
}

ISR(TIMER1_OVF_vect)
{

  //TCNT1 = 20000;        // at 16Mhz (0.5us) (65535-20000) = 22 ms (45Hz)

  //------------------------ESC i rise--------------------------
  if (!(ESC_stage %2))
  {
    if (PWM_OUT_PORT[ESC_stage / 2] >= 0)
      digitalWrite(PWM_OUT_PORT[ESC_stage / 2], HIGH);

    TCNT1 = 65535 - 2 * PWM_power_ready[ESC_stage / 2];    //1124us
    PWM_power_s[ESC_stage / 2] = PWM_power_ready[ESC_stage / 2];
    ESC_stage ++;
  }
  //------------------------ESC i fall--------------------------
  else
  {
    if (PWM_OUT_PORT[(ESC_stage - 1) / 2] >= 0)
      digitalWrite(PWM_OUT_PORT[(ESC_stage - 1) / 2], LOW);

    TCNT1 = 65535 - 2 * (PWM_PERIOD_4 - PWM_power_s[(ESC_stage - 1) / 2]);
    ESC_stage ++;
    ESC_stage = ESC_stage % 8;
  }   


  //------------------------Cheking for RC signal---------------
  if ((RC_IN_PORT[0] >= 0)&&(micros() > RC_check_time + PWM_PERIOD_4*5))
  {
    //if (RC_state && Init_complete) Serial.println("RC signal lost\n");
    RC_state = 0;
  }
  else 
  {
    //if (!RC_state && Init_complete) Serial.println("RC signal locked\n");
    RC_state = 1;
  }
}


void RC_read_init()
{
  if ( RC_IN_PORT[0] == 2)  
    attachInterrupt(0, RC_read_interrupt_1, CHANGE); 


  if ( RC_IN_PORT[1] == 3)  
    attachInterrupt(1, RC_read_interrupt_2, FALLING); 
}


void RC_read_interrupt_1()
{

  RC_Old[RC_select] = micros();

  RC_Time[RC_select - 1] = RC_Old[RC_select];
  RC_Time[RC_select - 1]-=RC_Old[RC_select - 1];
  RC_select ++ ;
  if (RC_select > NUM_RC_CHANNELS) RC_select = 1;
}

void RC_read_interrupt_2()
{
  RC_Time[6] = micros();
  RC_Time[6]-=RC_Old[6];
  RC_select = 1;
  RC_check_time = micros();
}

int RxGetChannelPulseWidth(int i)
{
  return (int) RC_Time[i];
}

void ServoPowerSet( int i, int Power)
{
  PWM_power_ready[i] = Power;
}








