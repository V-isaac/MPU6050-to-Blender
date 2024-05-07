#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"



#define Mux 0x70
// упрощеный I2C адрес нашего гироскопа/акселерометра MPU-6050.
MPU6050 mpu(0x68);

uint8_t error_code = 0U;
 
void MuxSelect(uint8_t i);
void giroscop_setup(uint8_t adr);

void setup() {
  Wire.setClock(200000);
  //Wire.setWireTimeout(250000, true);
  Serial.begin(115200);
  Serial.print("\n");
  /* Enable I2C */
  Wire.begin();//(22,23); //SDA //SCL

  for (int i = 0; i <= 5; i++) {
    MuxSelect(i);
    Serial.print("Initialising MPU #"); Serial.print(i); Serial.print("\n");
    //Initialisation and bunch of tests
    mpu.initialize();
    error_code = mpu.dmpInitialize();
    if (error_code == 1U) {
      Serial.print("Device "); Serial.print(i); Serial.print(" initialization failed: initial memory load failed. FUCK");
      while (1) {}
    }
    if (error_code == 2U) {
      Serial.print("Device "); Serial.print(i); Serial.print(" initialization failed: DMP configuration updates failed. FUCK");
      while (1) {}
    }
    if(!mpu.testConnection()){ Serial.print("FUCK connection failed. At least we've got this far"); }

    mpu.setDMPEnabled(true);
  }
  Serial.println("Success! We're past setup stage!");
}
 
void loop()
{ 
  for (uint8_t i = 0; i <= 5; i++){
    MuxSelect(i);
    uint8_t fifoBuffer[64];
    if(!mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
      { return; }
    Quaternion q;

    mpu.dmpGetQuaternion(&q, fifoBuffer);

    PrintKey(i);
    Serial.print(q.w);Serial.print(", ");
    Serial.print(q.x);Serial.print(", ");
    Serial.print(q.y);Serial.print(", ");
    Serial.print(q.z);Serial.print("]}\n");
  }
}

void PrintKey(uint8_t i){
  switch(i){
    case 0:
      Serial.print("{ \"key\": \"lh0\", \"value\": [");
      break;
    case 1:
      Serial.print("{ \"key\": \"lh1\", \"value\": ["); 
      break;
    case 2:
      Serial.print("{ \"key\": \"lh2\", \"value\": [");
      break;
    case 3:
      Serial.print("{ \"key\": \"lh3\", \"value\": [");
      break;
    case 4:
      Serial.print("{ \"key\": \"lh4\", \"value\": [");
      break;
    case 5:
      Serial.print("{ \"key\": \"lh5\", \"value\": [");
      break;
    default:
      Serial.print("FUCK");
      break;
  }
}
 
//Функция мультиплексирует адреса для обмена
void MuxSelect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(Mux);
  Wire.write(1 << i);
  Wire.endTransmission();
}
