#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

MPU6050 mpu(0x68);

void MuxSelect(uint8_t i);

void setup() {
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
    mpu.initialize();
    error_code = mpu.dmpInitialize();
    if (error_code == 1U) {
      Serial.print(" "); Serial.print(it); Serial.println(" memory.");
      errors++;
    }
    if (error_code == 2U) {
      Serial.print("Device "); Serial.print(it); Serial.println(" DMP update.");
      errors++;
    }
    if(!mpu.testConnection()){ Serial.println("Connection."); }

    mpu.setDMPEnabled(true);
  }

  if (errors != 0) {
    Serial.print("Amount of skill issues: "); Serial.print(errors); 
    while (1) {}
  }
  
  Serial.println("Lets go.");
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

  MuxSelect(incomingByte);
  Serial.println("accX accY accZ gyrX gyrY gyrZ");

  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6); // number of iterations
  Serial.print(mpu.getXAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getYAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getZAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getXGyroOffset()); Serial.print(", ");
  Serial.print(mpu.getYGyroOffset()); Serial.print(", ");
  Serial.print(mpu.getZGyroOffset());
  Serial.println("");
}

void MuxSelect(uint8_t refI) {
  if (refI > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << refI);
  Wire.endTransmission();
}
