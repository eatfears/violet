/* ******************************************************* */
/* I2C HMC5843 magnetometer                                */
/* ******************************************************* */

// Local magnetic declination
// I use this web : http://www.ngdc.noaa.gov/geomagmodels/Declination.jsp
#define MAGNETIC_DECLINATION -6.0    // not used now -> magnetic bearing

int CompassAddress = 0x42>>1;  //0x3C //0x3D;  //(0x42>>1);

void I2C_Init()
{
  Wire.begin();
}


void Compass_Init()
{
  Wire.beginTransmission(CompassAddress);
  Wire.write(0x77); 
  Wire.write(0x08);   
  Wire.write(0x52);// Set continouos mode (default to 10Hz)
  Wire.endTransmission(); //end transmission
}

void Read_Compass()
{
  int i = 0;
  byte buff[2];


  Wire.beginTransmission(0x43); 
  //Wire.send('A');        //sends address to read from
  Wire.endTransmission(); //end transmission


    //Wire.beginTransmission(CompassAddress); 
  Wire.requestFrom(CompassAddress, 2);    // request 6 bytes from device
  while(Wire.available()&&(i<2))   // ((Wire.available())&&(i<6))
  { 
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission(); //end transmission

    if (i==2)  // All bytes received?
  {
    // MSB byte first, then LSB, X,Y,Z
    MAG_Heading = buff[0];   // receive high byte (overwrites previous reading) 
    MAG_Heading = MAG_Heading * 256;     // shift high byte to be high 8 bits 
    MAG_Heading += buff[1];  // receive low byte as lower 8 bits 
    MAG_Heading /= 10;
    MAG_Heading = ToRad(MAG_Heading);
    //Serial.println(MAG_Heading);    // print the reading 
  }
  else
    Serial.println("!ERR: Mag data");
}


void Compass_Heading()
{
  /*
  float MAG_X;
   float MAG_Y;
   float cos_roll;
   float sin_roll;
   float cos_pitch;
   float sin_pitch;
   
   cos_roll = cos(roll);  // Optimizacion, se puede sacar esto de la matriz DCM?
   sin_roll = sin(roll);
   cos_pitch = cos(pitch);
   sin_pitch = sin(pitch);
   // Tilt compensated Magnetic filed X:
   MAG_X = magnetom_x*cos_pitch+magnetom_y*sin_roll*sin_pitch+magnetom_z*cos_roll*sin_pitch;
   // Tilt compensated Magnetic filed Y:
   MAG_Y = magnetom_y*cos_roll-magnetom_z*sin_roll;
   // Magnetic Heading
   MAG_Heading = atan2(-MAG_Y,MAG_X);
   mag_heading_x = cos(MAG_Heading);
   mag_heading_y = sin(MAG_Heading);*/
}
