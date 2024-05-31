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
    MuxSelect(it); // adress selection
    Serial.print("Инициализируем MPU#"); Serial.print(it); Serial.println("");
    //Initialisation and bunch of tests
    mpu.initialize();
    error_code = mpu.dmpInitialize(); 
    if (error_code == 1U) {
      Serial.print("Device "); Serial.print(it); Serial.println(" memory.");
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
    Serial.print("Skill issues: "); Serial.print(errors); 
    while (1) {}
  }
  
  Serial.println("Im done with this shit.");
}



void loop() { 
  for (uint8_t it = 0; it <= 7; it++){
    MuxSelect(it);

    if(!mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }

    Quaternion q;

    mpu.dmpGetQuaternion(&q, fifoBuffer);

    Serial.print("{ \"key\": \"lh"); Serial.print(it); Serial.print("\", \"value\": [");
    Serial.print(q.w);Serial.print(", ");
    Serial.print(q.x);Serial.print(", ");
    Serial.print(q.y);Serial.print(", ");
    Serial.print(q.z);Serial.print("]}\n");
  }
}
 

void MuxSelect(uint8_t i) {
  if (i > 7) return; 
 
  Wire.beginTransmission(0x70); 
  Wire.write(1 << i);
  Wire.endTransmission();
}
