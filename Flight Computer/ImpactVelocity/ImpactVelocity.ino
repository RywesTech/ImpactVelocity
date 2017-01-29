// Includes:
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Objects:
Servo parachuteMotor;
Servo airBrakes;
LSM9DS1 imu;
Adafruit_BMP280 bme;

// Variables:
float lastAccelRead = 0;
float altMeters = 0;
float altGoal = 236.22;
float altStart = 0;

// Indexes and statics:
int statusGreenPin = 2;
int statusRedPin = 3;
int parachuteMotorPin = 9;
int airBrakesPin = 10;

#define LSM9DS1_M   0x1E // 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // 0x6A if SDO_AG is LOW
#define DECLINATION -15.2 // Declination (degrees) in Portland, calc: http://www.ngdc.noaa.gov/geomag-web/#declination
#define MSL_PRESSURE 1032.84 // Pressure at sea level (change this)

void setup() {
  bool error; // keep track if there's any errors

  Serial.begin(9600);

  parachuteMotor.attach(parachuteMotorPin);
  airBrakes.attach(airBrakesPin);

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin()) {
    error = true;
  }

  if ( !bme.begin()) {
    error = true;
  }

  altStart = bme.readAltitude(MSL_PRESSURE);

  if (error) {
    statusError();
  } else {
    statusSuccess();
  }
}

void loop() {
  // Make sure all the shit here is non blocking

  if (imu.accelAvailable()) {
    imu.readAccel();
  }

  altMeters = bme.readAltitude(MSL_PRESSURE);

  if(altMeters > altGoal){
    //deploy brakes
  }

  Serial.println(altMeters - altStart);
}

void statusError() {
  digitalWrite(statusRedPin, HIGH);
  digitalWrite(statusGreenPin, LOW);
}

void statusSuccess() {
  digitalWrite(statusRedPin, LOW);
  digitalWrite(statusGreenPin, HIGH);
}

