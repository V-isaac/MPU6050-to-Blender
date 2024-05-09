// Get basic libraries
#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

#define Mux 0x70
// Simplified object to call 
MPU6050 mpu(0x68);

// Error code
uint8_t error_code = 0U, errors = 0;
 
// Initialise these first
void MuxSelect(uint8_t i);
void giroscop_setup(uint8_t adr);

void setup() {
  // Setting wire rate too hight may affect data integrity
  Wire.setClock(200000);
  // Selecting communication speed (must match with Python Script)
  Serial.begin(115200);
  Serial.print("\n");
 
  // Enable I2C
  Wire.begin(); //(SDA; SCL)

  // Setting up MPUs
  for (int i = 0; i <= 7; i++) {
    MuxSelect(i);
    Serial.print("Initialising MPU #"); Serial.print(i); Serial.print("\n");
    //Initialisation and bunch of tests
    mpu.initialize();
    error_code = mpu.dmpInitialize();
    if (error_code == 1U) {
      Serial.print("Device "); Serial.print(i); Serial.print(" initialization failed: initial memory load failed. FUCK");
			errors++;
    }
    if (error_code == 2U) {
      Serial.print("Device "); Serial.print(i); Serial.print(" initialization failed: DMP configuration updates failed. FUCK");
			errors++;
    }
    if(!mpu.testConnection()){ Serial.print("FUCK connection failed. At least we've got this far"); }

    mpu.setDMPEnabled(true);
  }

	if (errors != 0) {
    Serial.print("Amount of errors: "); Serial.print(errors); 
    while (1) {}
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

    Serial.print("{ \"key\": \"lh"); Serial.print(i); Serial.print("\", \"value\": [");
    Serial.print(q.w);Serial.print(", ");
    Serial.print(q.x);Serial.print(", ");
    Serial.print(q.y);Serial.print(", ");
    Serial.print(q.z);Serial.print("]}\n");
  }
}

//function to select Multiplexor route
void MuxSelect(uint8_t i) {
  if (i > 7) return; 
 
  Wire.beginTransmission(Mux);
  Wire.write(1 << i);
  Wire.endTransmission();
}
