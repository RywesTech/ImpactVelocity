//
//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//               Buddha bless the code
//
//     Hope we dont have anohter launch 1 failuare.
//         (Cough cough "learning expereince")
//

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
File data;

bool setupError, error; // keep track if we have an error

// Variables:
float bmeTemp, bmePressure, bmeAlt;
float imuGX, imuGY, imuGZ;
float imuAX, imuAY, imuAZ;
float imuMX, imuMY, imuMZ;
int battPercent, battRaw;
float altStart;
String filename; // Datalogging
int iterations;  // Datalogging
long timeMillis;

// Indexes and statics:
int statusGreenPin = 13;
int statusRedPin = 11;

#define LSM9DS1_M   0x1E // 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // 0x6A if SDO_AG is LOW
#define DECLINATION -15.2 // Declination (degrees) in Portland, calc: http://www.ngdc.noaa.gov/geomag-web/#declination
#define MSL_PRESSURE 1000.38 // Pressure at sea level (change this)

void setup() {

  Serial.begin(9600);
  Serial.println("init...");

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin()) {
    setupError = true;
  }

  if (!bme.begin()) {
    setupError = true;
  }

  if (!SD.begin(4)) { // 4 is chip select pin
    setupError = true;
  }

  bool foundFile = false;
  //find new filename here:
  filename = "data4.csv"; // or not (for now)
  
  data = SD.open(filename, FILE_WRITE); // create the file
  data.close();

}

void loop() {

  String logString;

  if (imu.gyroAvailable()) {
    imu.readGyro();
  } else {
    error = true;
  }
  
  if (imu.accelAvailable()) {
    imu.readAccel();
  } else {
    error = true;
  }

  if (imu.magAvailable()) {
    imu.readMag();
  } else {
    error = true;
  }

  timeMillis  = millis();
  
  bmeTemp     = bme.readTemperature();
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

  battRaw     = analogRead(A3);
  battPercent = map(battRaw, 660, 860, 0, 100);

  logString = String(timeMillis) + "," +
    String(bmeTemp)     + "," +
    String(bmePressure) + "," +
    String(bmeAlt)      + "," +
    
    String(imuGX) + "," +
    String(imuGY) + "," +
    String(imuGZ) + "," +

    String(imuAX) + "," +
    String(imuAY) + "," +
    String(imuAZ) + "," +

    String(imuMX) + "," +
    String(imuMY) + "," +
    String(imuMZ) + "," +

    String(battRaw) + "," +
    String(battPercent);
    
  logData(logString);

  if (error || setupError) {
    statusBoi();
  } else {
    statusSpicy();
  }
}

void logData(String dataString) { // Log our data to SD card
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening " + filename);
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
