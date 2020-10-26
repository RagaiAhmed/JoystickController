#include <Wire.h>  // To use I2C
#include <SoftwareSerial.h>  // To use software serial
SoftwareSerial mySerial(2, 3); // RX, TX

// MPU sensor I2C slave address
#define MPU_I2C_ADDR 104  

// MPU wakeup register address
#define MPU_WAKE 107  

// MPU first register address
#define START 59  

// Pin connected to pressure sensor
#define pressurePin A1


// Stores the yaw angle
double ax = 0 ;
double ay = 0 ;
double az = 0 ;
double gx = 0 ;
double gy = 0 ;
double gz = 0 ;

// Value to be multiplied to the analog pressure reading
double pressureFactor = 5/1024*100;



void setup() 
{
  pinMode(pressurePin,INPUT);
  
  Wire.begin();  // Initializes I2C communication as master
  Serial.begin(9600);
  mySerial.begin(9600);
  
  // Change sleep mode of the IMU sensor
  Wire.beginTransmission(MPU_I2C_ADDR);
  Wire.write(MPU_WAKE);  // Selects register
  Wire.write(0);  // Sends a zero byte
  Wire.endTransmission();
}


unsigned long long last_update = 0;
unsigned long long UpdatePeriod = 60;
void loop() 
{
  if(millis() - last_update >= UpdatePeriod)
  {
        Wire.beginTransmission(MPU_I2C_ADDR);
        Wire.write(START);  // Selects register
        Wire.endTransmission();
        
        Wire.requestFrom(MPU_I2C_ADDR, 14);  // Requests 2 bytes

        // Combine the two bytes and divides by the scale factor for default sensitivity according to the datasheet 
        double aX = (Wire.read() << 8 | Wire.read()) / 16384.0; 
        double aY = (Wire.read() << 8 | Wire.read()) / 16384.0; 
        double aZ = (Wire.read() << 8 | Wire.read()) / 16384.0; 
        Wire.read();Wire.read();
        double gX = (Wire.read() << 8 | Wire.read()) / 131.0; 
        double gY = (Wire.read() << 8 | Wire.read()) / 131.0; 
        double gZ = (Wire.read() << 8 | Wire.read()) / 131.0; 
        
        // Adds up the change 
        ax += aX * UpdatePeriod / 1000.0L;
        ay += aY * UpdatePeriod / 1000.0L;
        az += aZ * UpdatePeriod / 1000.0L;

        gx += gX * UpdatePeriod / 1000.0L;
        gy += gY * UpdatePeriod / 1000.0L;
        gz += gZ * UpdatePeriod / 1000.0L;

        // Normalize the yaw_angle
        while(gx<0) gx+=360;  // Make it a positive equivalent
        while(gx>180) gx -=360;  // Make it negative for angles bigger than 180

        while(gy<0) gx+=360;  // Make it a positive equivalent
        while(gy>180) gx -=360;  // Make it negative for angles bigger than 180
       
        while(gz<0) gx+=360;  // Make it a positive equivalent
        while(gz>180) gx -=360;  // Make it negative for angles bigger than 180
    
        int pressureVal = analogRead(pressurePin)*pressureFactor;

        String str;
        str += "X"+String(gx);
        str += "Y"+String(gy);
        str += "Z"+String(gz);
        str += "x"+String(ax);
        str += "y"+String(ay);
        str += "z"+String(az);
        str += "P"+String(pressureVal);

        Serial.write(str.c_str());
        last_update = millis();  // Stores update time
  }

  while(Serial.available())mySerial.write(Serial.read());

}
