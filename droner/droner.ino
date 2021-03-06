#include <Wire.h>
#include <Servo.h>
#include <PID_v1.h>

// globals
// GY-91
int address_MPU9250 = 0x68;         //MPU9250
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

// resistor pot & ESC
int analogPin = A0;
int val = 0;
Servo ESC;
int potValue;

// PID algorithm
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,.001,0.0006,0.0002, REVERSE);


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(19200);
  Serial.println("setup droner code");

  // initialize gyroscope
  // set power mode
  Wire.begin();
  Wire.beginTransmission(address_MPU9250);
  Wire.write(0x6B);                           
  Wire.write(0b00000000);
  Wire.endTransmission();
  // set accel DLPF  (1D)
  Wire.begin();
  Wire.beginTransmission(address_MPU9250);
  Wire.write(0x1D);                           
  Wire.write(0b00000101);
  Wire.endTransmission();

  // initialize PID
  Setpoint = 0;
  Input = 0;
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(50);
  myPID.SetOutputLimits(0, 180);
    
  // initialize ESC module
  ESC.attach(9,1000,2000); // (pin, min pulse width, max pulse width in microseconds)
  ESC.write(0);
}

// the loop function runs over and over again forever
void loop() {

  // reading the gyroscope
  Wire.beginTransmission(address_MPU9250);
  if(Wire.endTransmission() == 0)
  {



    
    Wire.beginTransmission(address_MPU9250);    // access GY-91
    Wire.write(0x43);                           // starting register of gyroscope
    Wire.endTransmission();
    Wire.requestFrom(address_MPU9250, 6);       // retrieve gyroscope values
    while(Wire.available() < 6);
    gx = Wire.read()<<8|Wire.read();
    gy = Wire.read()<<8|Wire.read();
    gz = Wire.read()<<8|Wire.read();

    Wire.beginTransmission(address_MPU9250);    // access GY-91
    Wire.write(0x3B);                           // starting register of gyroscope
    Wire.endTransmission();
    Wire.requestFrom(address_MPU9250, 6);       // retrieve gyroscope values
    while(Wire.available() < 6);
    ax = Wire.read()<<8|Wire.read();
    ay = Wire.read()<<8|Wire.read();
    az = Wire.read()<<8|Wire.read();
    
    //Serial.print( "GYRO XOUT = " );
    //Serial.print( gx );
    //Serial.print( " + " );
    //Serial.print( gy );
    //Serial.print( " + " );
    //Serial.print( gz );
    //Serial.print( "\n" );

    //float rr = ay-az;

    Serial.print( "ACCEL YOUT = " );
    //Serial.print( ax );
    //Serial.print( " + " );
    Serial.print( ay );  // KEEP LEVEL
    //Serial.print( " + " );
    //Serial.print( az );
    Serial.print( "\n" );





    
  }

delay(50);

  Input = ay;
  myPID.Compute();
  Serial.print( "output = " );
  Serial.print( Output );
  Serial.print( "\n" );

ESC.write( Output );


  // reading the resistor pot and writing the XPS
  //delay(250);                       // wait for a second
  val = analogRead(analogPin);
  if(val > 15)
  {
    potValue = map(val, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
    ESC.write(potValue);    // Send the signal to the ESC
    //Serial.println( val );
  }


}
