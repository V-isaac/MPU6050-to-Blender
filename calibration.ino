#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

MPU6050 mpu0(0x68);
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x68);
MPU6050 mpu3(0x68);
MPU6050 mpu4(0x68);
MPU6050 mpu5(0x68);
MPU6050 mpu6(0x68);
MPU6050 mpu7(0x68);
MPU6050 mpu8(0x68);
// a lot of objects, one adress
// solving it with IIC mux we have in a project

char FUCK[] = "FUCK ";

void MuxSelect(uint8_t i);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 

  /* Enable I2C */
  Wire.setClock(200000);
  Wire.begin();//(22,23); //SDA //SCL
  Serial.println("");

  uint8_t error_code = 0U, errors = 0;

  for (uint8_t it = 0; it <= 7; it++) {
    MuxSelect(it); // Выбор адреса
    Serial.print("MPU#"); Serial.print(it); Serial.println("");
    //Initialisation and bunch of tests
    switch(it){
      case (0):
        mpu0.initialize();
        error_code = mpu0.dmpInitialize();
        break;
      case (1):
        mpu1.initialize();
        error_code = mpu1.dmpInitialize();
        break;
      case (2):
        mpu2.initialize();
        error_code = mpu2.dmpInitialize();
        break;
       case (3):
        mpu3.initialize();
        error_code = mpu2.dmpInitialize();
        break;
      case (4):
        mpu4.initialize();
        error_code = mpu2.dmpInitialize();
        break;
      case (5):
        mpu5.initialize();
        error_code = mpu2.dmpInitialize();
        break;
      case (6):
        mpu6.initialize();
        error_code = mpu2.dmpInitialize();
        break;
      case (7):
        mpu7.initialize();
        error_code = mpu2.dmpInitialize();
        break;
      case (8):
        mpu8.initialize();
        error_code = mpu2.dmpInitialize();
        break;
    }

    if (error_code == 1U) {
      Serial.print(FUCK); Serial.print(it);
      errors++;
    }
    if (error_code == 2U) {
      Serial.print(FUCK); Serial.print(it);
      errors++;
    }
    switch(it){
      case (0):
        if(!mpu0.testConnection()){ Serial.print(FUCK); break;}
        mpu0.setDMPEnabled(true);
        break;
      case (1):
        if(!mpu1.testConnection()){ errors++; break;}
        mpu1.setDMPEnabled(true);
        break;
      case (2):
        if(!mpu2.testConnection()){ errors++; break;}
        mpu2.setDMPEnabled(true);
        break;
      case (3):
        if(!mpu3.testConnection()){ errors++; break;}
        mpu3.setDMPEnabled(true);
        break;
      case (4):
        if(!mpu4.testConnection()){ errors++; break;}
        mpu4.setDMPEnabled(true);
        break;
      case (5):
        if(!mpu5.testConnection()){ errors++; break;}
        mpu5.setDMPEnabled(true);
        break;
      case (6):
        if(!mpu6.testConnection()){ errors++; break;}
        mpu6.setDMPEnabled(true);
        break;
      case (7):
        if(!mpu7.testConnection()){ errors++; break;}
        mpu7.setDMPEnabled(true);
        break;
      case (8):
        if(!mpu8.testConnection()){ errors++; break;}
        mpu8.setDMPEnabled(true);
        break;
      }
  }

  if (errors != 0) {
    Serial.print("Количество ошибок: "); Serial.print(errors);
    while (1) {}
  }
}

char incomingByte;

void loop() {
  while(1){
    if (Serial.available() > 0) {
      incomingByte = Serial.read();

      if(isdigit(incomingByte))
        break;
    }
  }
  //reads serial for accel to calibrate
  //one at a time

  MuxSelect(incomingByte);

  switch(incomingByte){
      case (0):
        mpu0.CalibrateAccel(6);
        mpu0.CalibrateGyro(6); // number of iterations
        break;
      case (1):
        mpu1.CalibrateAccel(6);
        mpu1.CalibrateGyro(6); // number of iterations
        break;
      case (2):
        mpu2.CalibrateAccel(6);
        mpu2.CalibrateGyro(6); // number of iterations
        break;
      case (3):
        mpu3.CalibrateAccel(6);
        mpu3.CalibrateGyro(6); // number of iterations
        break;
      case (4):
        mpu4.CalibrateAccel(6);
        mpu4.CalibrateGyro(6); // number of iterations
        break;
      case (5):
        mpu5.CalibrateAccel(6);
        mpu5.CalibrateGyro(6); // number of iterations
        break;
      case (6):
        mpu6.CalibrateAccel(6);
        mpu6.CalibrateGyro(6); // number of iterations
        break;
      case (7):
        mpu7.CalibrateAccel(6);
        mpu7.CalibrateGyro(6); // number of iterations
        break;
      case (8):
        mpu8.CalibrateAccel(6);
        mpu8.CalibrateGyro(6); // number of iterations
        break;
      default:
        Serial.print("out of range");
        break;
  }
  Serial.println("");
}

void MuxSelect(uint8_t refI) {
  if (refI > 7) return; 
 
  Wire.beginTransmission(0x70); 
  Wire.write(1 << refI); 
  Wire.endTransmission(); 
}
