// Includes:
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Objects:
LSM9DS1 imu;
Adafruit_BMP280 bme;

bool error; // keep track if we have an error

// Variables:
float bmeTemp, bmePressure, bmeAlt;
float imuGX, imuGY, imuGZ;
float imuAX, imuAY, imuAZ;
float imuMX, imuMY, imuMZ;
float altStart = 0;

// Indexes and statics:
int statusGreenPin = 2;
int statusRedPin = 3;

#define LSM9DS1_M   0x1E // 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // 0x6A if SDO_AG is LOW
#define DECLINATION -15.2 // Declination (degrees) in Portland, calc: http://www.ngdc.noaa.gov/geomag-web/#declination
#define MSL_PRESSURE 1032.84 // Pressure at sea level (change this)

void setup() {

  Serial.begin(9600);

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin()) {
    error = true;
  }

  if (!bme.begin()) {
    error = true;
  }

}

void loop() {

  String logString;
  error = false;
  
  if (imu.accelAvailable()) {
    imu.readAccel();
  }else{
    error = true;
  }
  
  bmeTemp = bme.readTemperature();
  bmePressure = bme.readPressure();
  bmeAlt      = bme.readAltitude(MSL_PRESSURE);

  imuGX = imu.calcGyro(imu.gx);
  imuGY = imu.calcGyro(imu.gy);
  imuGZ = imu.calcGyro(imu.gz);
  
  imuAX = imu.calcAccel(imu.ax);
  imuAY = imu.calcAccel(imu.ay);
  imuAZ = imu.calcAccel(imu.az);

  imuMX = imu.calcMag(imu.mx);
  imuMY = imu.calcMag(imu.my);
  imuMZ = imu.calcMag(imu.mz);

  Serial.println(bmeTemp);
  Serial.println(imuAZ);

  logData(logString);

  if (error) {
    statusBoi();
  } else {
    statusSpicy();
  }
}

void logData(String dataString){ // Log our data to SD card
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
    error = true;
  }
}

void statusBoi() { // Not good
  digitalWrite(statusRedPin, HIGH);
  digitalWrite(statusGreenPin, LOW);
}

void statusSpicy() { // We ready to roll
  digitalWrite(statusRedPin, LOW);
  digitalWrite(statusGreenPin, HIGH);
}
