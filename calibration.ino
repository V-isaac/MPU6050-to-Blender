#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

// Change to true or false
// Note that Custom Calibration has priority
#define LIBRARY_EXAMPLE true
#define CUSTOM_CALIBRATION false
// Some variables for calibration
#define BUFFER_SIZE 100
#define CALIBRATION_ITERATIONS 10 //10 should be good enough. Do NOT set higher than 15

#if CUSTOM_CALIBRATION
// If using custom calibration, compile nothing
#elif LIBRARY_EXAMPLE
int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz, state = 0;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
const int acel_deadzone = 10;  // accuracy of calibration (default 8)
const int gyro_deadzone = 6;
#else
#endif

MPU6050 mpu(0x68);

void MuxSelect(uint8_t i);

void setup() {
  Serial.begin(115200);

  /* Enable I2C */
  Wire.setClock(200000);
  Wire.begin(); //SDA //SCL
  Serial.println("");

  uint8_t error_code = 0U, errors = 0;

  for (uint8_t it = 0; it <= 7; it++) {
    MuxSelect(it); // Address select
    Serial.print("MPU#"); Serial.print(it); Serial.println("");
    //Initialisation and bunch of tests
    mpu.initialize();
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); //Setting default ranges, they're good enough
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
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
    Serial.print("Amount of skill issues: "); Serial.print(errors); 
    while (1) {}
  }
  
  Serial.println("Lets go.");
}
char incomingByte;

void loop() {
  // Waiting for input
  // Type in serial any number from 0 to 7 to start calibrating this device
  while(1){
    if (Serial.available() > 0) {
      incomingByte = Serial.read();

      if(isdigit(incomingByte))
        break;
    }
  }

  MuxSelect(incomingByte);
  Serial.print("Device #"); Serial.println(incomingByte);
  Serial.println("accX accY accZ gyrX gyrY gyrZ");

  // Setting all offsets to 0
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  #if CUSTOM_CALIBRATION // Using custom calibration first
  long offsets[6];
  long offsetsOld[6];
  int16_t mpuGet[6];
  
  for (byte n = 0; n < CALIBRATION_ITERATIONS; n++) {
    for (byte j = 0; j < 6; j++) {    // resetting calibration array
      offsets[j] = 0;
    }
    for (byte i = 0; i < 100 + BUFFER_SIZE; i++) {
      // making BUFFER_SIZE readings
      mpu.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);
      if (i >= 99) {                         // Ditching first readings
        for (byte j = 0; j < 6; j++) {
          offsets[j] += (long)mpuGet[j];   // reading into array
        }
      }
    }
    for (byte i = 0; i < 6; i++) {
      offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE); // using old calibration, if exists
      if (i == 2) offsets[i] += 16384;                               // if Z axis, then assigning 16384 (see MPU6050 datasheet)
      offsetsOld[i] = offsets[i];
    }
  }

  for (int i = 0; i < 6; i++) {
  Serial.print(offsets[i]); Serial.print("\n");
  }
  Serial.println();
  #elif LIBRARY_EXAMPLE
  // Calibrating via example from MPU library. Should be more accurate, but also longer
  // DO NOT: MOVE SENSOR, TOUCH IT, HAVE ANYTHING VIBRATE NEARBY 
  // AND MOST IMPORTANTLY DON'T BREATHE
  
  Serial.println("Reading sensors for the first time...");
  meansensors();
  Serial.println("Calibrating...");
  calibration();
  Serial.print("Your offsets:\t");
  Serial.print(ax_offset);
  Serial.print(", ");
  Serial.print(ay_offset);
  Serial.print(", ");
  Serial.print(az_offset);
  Serial.print(", ");
  Serial.print(gx_offset);
  Serial.print(", ");
  Serial.print(gy_offset);
  Serial.print(", ");
  Serial.println(gz_offset);

  
  #else // Calibrating trough function. WILL set calibration values for you. We just retrieve them
  mpu.CalibrateAccel(CALIBRATION_ITERATIONS);
  mpu.CalibrateGyro(CALIBRATION_ITERATIONS); // Number of iterations
  Serial.print(mpu.getXAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getYAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getZAccelOffset()); Serial.print(", ");
  Serial.print(mpu.getXGyroOffset()); Serial.print(", ");
  Serial.print(mpu.getYGyroOffset()); Serial.print(", ");
  Serial.print(mpu.getZGyroOffset());
  #endif
    
  Serial.println("IM DONE WITH THIS SHIT");
  Serial.println("");
}

void MuxSelect(uint8_t refI) {
  if (refI > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << refI);
  Wire.endTransmission();
}

#if CUSTOM_CALIBRATION
#elif LIBRARY_EXAMPLE
void meansensors() {
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
  while (i < (CALIBRATION_ITERATIONS + 101)) { // read raw data
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    if (i > 100 && i <= (CALIBRATION_ITERATIONS + 100)) { // Ditch first measurements
      buff_ax = buff_ax + ax;
      buff_ay = buff_ay + ay;
      buff_az = buff_az + az;
      buff_gx = buff_gx + gx;
      buff_gy = buff_gy + gy;
      buff_gz = buff_gz + gz;
    }
    if (i == (CALIBRATION_ITERATIONS + 100)) {
      mean_ax = buff_ax / CALIBRATION_ITERATIONS;
      mean_ay = buff_ay / CALIBRATION_ITERATIONS;
      mean_az = buff_az / CALIBRATION_ITERATIONS;
      mean_gx = buff_gx / CALIBRATION_ITERATIONS;
      mean_gy = buff_gy / CALIBRATION_ITERATIONS;
      mean_gz = buff_gz / CALIBRATION_ITERATIONS;
    }
    i++;
    delay(2);
  }
}

void calibration() {
  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = (16384 - mean_az) / 8;
  gx_offset = -mean_gx / 4;
  gy_offset = -mean_gy / 4;
  gz_offset = -mean_gz / 4;
  while (1) {
    int ready = 0;
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);
    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);
    meansensors();
    Serial.println("...");
    if (abs(mean_ax) <= acel_deadzone) ready++;
    else ax_offset = ax_offset - mean_ax / acel_deadzone;
    if (abs(mean_ay) <= acel_deadzone) ready++;
    else ay_offset = ay_offset - mean_ay / acel_deadzone;
    if (abs(16384 - mean_az) <= acel_deadzone) ready++;
    else az_offset = az_offset + (16384 - mean_az) / acel_deadzone;
    if (abs(mean_gx) <= gyro_deadzone) ready++;
    else gx_offset = gx_offset - mean_gx / (gyro_deadzone + 1);
    if (abs(mean_gy) <= gyro_deadzone) ready++;
    else gy_offset = gy_offset - mean_gy / (gyro_deadzone + 1);
    if (abs(mean_gz) <= gyro_deadzone) ready++;
    else gz_offset = gz_offset - mean_gz / (gyro_deadzone + 1);
    if (ready == 6) break;
  }
}
#else
#endif
