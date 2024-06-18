//#include <MadgwickAHRSQ.h>
#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

#define TIME_GYRO 2000            // dt for integration

//Madgwick filter;

// упрощеный I2C адрес нашего гироскопа/акселерометра MPU-6050.
MPU6050 mpu0(0x68);
MPU6050 mpu1(0x68);
MPU6050 mpu2(0x68);
MPU6050 mpu3(0x68);
MPU6050 mpu4(0x68);
MPU6050 mpu5(0x68);
MPU6050 mpu6(0x68);
MPU6050 mpu7(0x68);

//Store repeating chars to save up mem space
char dev[] = "Device ";
char imem[] = "Mem load";
char idmp[] = "DMP update";
char icon[] = "Connection";
char comaSpace[] = ", ";
char closeJSON[] = "]}\n";

void MuxSelect(uint8_t i);

// Allocating memory to rewrite each time
uint8_t fifoBuffer[64];  // FIFO buffer array
uint8_t it;              // Variable for iterations
// float deltat = 0.0f;                              // For filter schemes
// uint32_t lastUpdate = 0, firstUpdate = 0;         // used to calculate integration interval
// uint32_t Now = 0;

void setup() {

  //Wire.setWireTimeout(250000, true);
  Serial.begin(115200);  // Запуск общения по USB

  /* Enable I2C */
  Wire.setClock(200000);  // Запуск общения по IIC
  Wire.begin();           //(22,23); //SDA //SCL
  Serial.println("");

  uint8_t error_code = 0U, errors = 0;  // Локальные переменные для фиксации ошибок
  for (int i = 0; i <= 7; i++) {
    MuxSelect(i);  // already handles out of bounds, just in case
    Serial.print(dev);
    Serial.print(i);
    Serial.println("");

    switch (i) {
      case 0:
        mpu0.initialize();
        error_code = mpu0.dmpInitialize();
        if (!mpu0.testConnection()) { Serial.println(icon); }
        mpu0.setDMPEnabled(true);
        mpu0.setXAccelOffset(-1913);
        mpu0.setYAccelOffset(932);
        mpu0.setZAccelOffset(1969);
        mpu0.setXGyroOffset(152);
        mpu0.setYGyroOffset(-30);
        mpu0.setZGyroOffset(-62);
        break;
      case 1:
        mpu1.initialize();
        error_code = mpu1.dmpInitialize();
        if (!mpu1.testConnection()) { Serial.println(icon); }
        mpu1.setDMPEnabled(true);
        mpu1.setXAccelOffset(-3590);
        mpu1.setYAccelOffset(-2824);
        mpu1.setZAccelOffset(1348);
        mpu1.setXGyroOffset(160);
        mpu1.setYGyroOffset(82);
        mpu1.setZGyroOffset(-41);
        break;
      case 2:
        mpu2.initialize();
        error_code = mpu2.dmpInitialize();
        if (!mpu2.testConnection()) { Serial.println(icon); }
        mpu2.setDMPEnabled(true);
        mpu2.setXAccelOffset(-328);
        mpu2.setYAccelOffset(689);
        mpu2.setZAccelOffset(1445);
        mpu2.setXGyroOffset(83);
        mpu2.setYGyroOffset(-48);
        mpu2.setZGyroOffset(-41);
        break;
      case 3:
        mpu3.initialize();
        error_code = mpu3.dmpInitialize();
        if (!mpu3.testConnection()) { Serial.println(icon); }
        mpu3.setDMPEnabled(true);
        mpu3.setXAccelOffset(169);
        mpu3.setYAccelOffset(147);
        mpu3.setZAccelOffset(1070);
        mpu3.setXGyroOffset(-8);
        mpu3.setYGyroOffset(-62);
        mpu3.setZGyroOffset(-87);
        break;
      case 4:
        mpu4.initialize();
        error_code = mpu4.dmpInitialize();
        if (!mpu4.testConnection()) { Serial.println(icon); }
        mpu4.setDMPEnabled(true);
        mpu4.setXAccelOffset(-4192);
        mpu4.setYAccelOffset(-1335);
        mpu4.setZAccelOffset(1195);
        mpu4.setXGyroOffset(-15);
        mpu4.setYGyroOffset(36);
        mpu4.setZGyroOffset(-1847);
        break;
      case 5:
        mpu5.initialize();
        error_code = mpu5.dmpInitialize();
        if (!mpu5.testConnection()) { Serial.println(icon); }
        mpu5.setDMPEnabled(true);
        mpu5.setXAccelOffset(-2199);
        mpu5.setYAccelOffset(699);
        mpu5.setZAccelOffset(1670);
        mpu5.setXGyroOffset(21);
        mpu5.setYGyroOffset(105);
        mpu5.setZGyroOffset(51);
        break;
      case 6:
        mpu6.initialize();
        error_code = mpu6.dmpInitialize();
        if (!mpu6.testConnection()) { Serial.println(icon); }
        mpu6.setDMPEnabled(true);
        mpu6.setXAccelOffset(-3372);
        mpu6.setXAccelOffset(-395);
        mpu6.setYAccelOffset(1586);
        mpu6.setXGyroOffset(89);
        mpu6.setYGyroOffset(-41);
        mpu6.setZGyroOffset(29);
        break;
      case 7:
        mpu7.initialize();
        error_code = mpu7.dmpInitialize();
        if (!mpu7.testConnection()) { Serial.println(icon); }
        mpu7.setDMPEnabled(true);
        mpu7.setXAccelOffset(-1780);
        mpu7.setYAccelOffset(-1450);
        mpu7.setZAccelOffset(1488);
        mpu7.setXGyroOffset(108);
        mpu7.setYGyroOffset(-50);
        mpu7.setZGyroOffset(-23);
        break;
    }

    if (error_code == 1U) {
      Serial.print(dev);
      Serial.print(i);
      Serial.println(imem);
      errors++;
    }
    if (error_code == 2U) {
      Serial.print(dev);
      Serial.print(i);
      Serial.println(idmp);
      errors++;
    }
  }

  if (errors != 0) {
    Serial.print("Количество ошибок: ");
    Serial.print(errors);
    while (1) {}
  }

  Serial.println();
  Serial.println();
  Serial.println();
}

// Variables for sensors
int16_t acc[3], gyr[3];
float angle_fx[8], angle_fy[8], angle_fz[8];  // углы, рассчитанные по фильтру
Quaternion q0, q1, q2, q3, q4, q5, q6, q7;
long int time_timer;
//Quaternion q;

void loop() {
  unsigned long microsNow, microsPrevious = 0, microsPerReading = 1000000 / 25;


  for (it = 0; it <= 7; it++) {
    MuxSelect(it);

    switch (it) {
      case 0:
        if (!mpu0.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 1:
        if (!mpu1.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 2:
        if (!mpu2.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 3:
        if (!mpu3.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 4:
        if (!mpu4.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 5:
        if (!mpu5.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 6:
        if (!mpu6.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
      case 7:
        if (!mpu7.dmpGetCurrentFIFOPacket(fifoBuffer)) { return; }
        break;
    }

    Serial.print("{ \"key\": \"lh");
    Serial.print(it);
    Serial.print("\", \"value\": [");  // Start JSON formating

    microsNow = micros();
    if (microsNow - microsPrevious >= microsPerReading) {
    if (time_timer < micros()) {
    time_timer = micros() + TIME_GYRO;
    
    switch (it) {
      case 0:
        //mpu0.dmpGetQuaternion(&q, fifoBuffer);
        mpu0.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q0.w, &q0.x, &q0.y, &q0.z);
        Serial.print(q0.w);        Serial.print(comaSpace);
        Serial.print(q0.x);        Serial.print(comaSpace);
        Serial.print(q0.y);        Serial.print(comaSpace);
        Serial.print(q0.z);
        break;
      case 1:
        //mpu1.dmpGetQuaternion(&q1, fifoBuffer);
        mpu1.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q1.w, &q1.x, &q1.y, &q1.z);
        Serial.print(q1.w);        Serial.print(comaSpace);
        Serial.print(q1.x);        Serial.print(comaSpace);
        Serial.print(q1.y);        Serial.print(comaSpace);
        Serial.print(q1.z);
        break;
      case 2:
        //mpu2.dmpGetQuaternion(&q2, fifoBuffer);
        mpu2.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q2.w, &q2.x, &q2.y, &q2.z);
        Serial.print(q2.w);        Serial.print(comaSpace);
        Serial.print(q2.x);        Serial.print(comaSpace);
        Serial.print(q2.y);        Serial.print(comaSpace);
        Serial.print(q2.z);
        break;
      case 3:
        //mpu3.dmpGetQuaternion(&q3, fifoBuffer);
        mpu3.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q3.w, &q3.x, &q3.y, &q3.z);
        Serial.print(q3.w);        Serial.print(comaSpace);
        Serial.print(q3.x);        Serial.print(comaSpace);
        Serial.print(q3.y);        Serial.print(comaSpace);
        Serial.print(q3.z);
        break;
      case 4:
        //mpu4.dmpGetQuaternion(&q4, fifoBuffer);
        mpu4.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q4.w, &q4.x, &q4.y, &q4.z);
        Serial.print(q4.w);        Serial.print(comaSpace);
        Serial.print(q4.x);        Serial.print(comaSpace);
        Serial.print(q4.y);        Serial.print(comaSpace);
        Serial.print(q4.z);
        break;
      case 5:
        //mpu5.dmpGetQuaternion(&q5, fifoBuffer);
        mpu5.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q5.w, &q5.x, &q5.y, &q5.z);
        Serial.print(q5.w);        Serial.print(comaSpace);
        Serial.print(q5.x);        Serial.print(comaSpace);
        Serial.print(q5.y);        Serial.print(comaSpace);
        Serial.print(q5.z);
        break;
      case 6:
        //mpu6.dmpGetQuaternion(&q6, fifoBuffer);
        mpu6.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q6.w, &q6.x, &q6.y, &q6.z);
        Serial.print(q6.w);        Serial.print(comaSpace);
        Serial.print(q6.x);        Serial.print(comaSpace);
        Serial.print(q6.y);        Serial.print(comaSpace);
        Serial.print(q6.z);
        break;
      case 7:
        //mpu7.dmpGetQuaternion(&q7, fifoBuffer);
        mpu7.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
        GetAngleFiltr(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2], it);
        ToQuaternion(&angle_fx[it], &angle_fy[it], &angle_fz[it], &q7.w, &q7.x, &q7.y, &q7.z);
        Serial.print(q7.w);        Serial.print(comaSpace);
        Serial.print(q7.x);        Serial.print(comaSpace);
        Serial.print(q7.y);        Serial.print(comaSpace);
        Serial.print(q7.z);
        break;
    }

    /*Serial.print(q.w);Serial.print(comaSpace);
    Serial.print(q.x);Serial.print(comaSpace);
    Serial.print(q.y);Serial.print(comaSpace);
    Serial.print(q.z);*/
    Serial.print(closeJSON);  // stop JSON packet
    }
  }
  microsPrevious = microsPrevious + microsPerReading;
  }
}

//Функция мультиплексирует адреса для обмена данными
void MuxSelect(uint8_t it) {
  if (it > 7) return;  // Если выходим за пределы адресов - не взаимодействуем

  Wire.beginTransmission(0x70);  // Начинаем общение по данному адресу
  Wire.write(1 << it);           // Сместить Истину на определённый регистр
  Wire.endTransmission();        // Закончить общение
}


void ToQuaternion(float* roll, float* pitch, float* yaw, float* q1, float* q2, float* q3, float* q4)  // roll (x), pitch (y), yaw (z), angles are in radians
{
  *q1 = cos(*roll * 0.5) * cos(*pitch * 0.5) * cos(*yaw * 0.5) + sin(*roll * 0.5) * sin(*pitch * 0.5) * sin(*yaw * 0.5);
  *q2 = sin(*roll * 0.5) * cos(*pitch * 0.5) * cos(*yaw * 0.5) - cos(*roll * 0.5) * sin(*pitch * 0.5) * sin(*yaw * 0.5);
  *q3 = cos(*roll * 0.5) * sin(*pitch * 0.5) * cos(*yaw * 0.5) + sin(*roll * 0.5) * cos(*pitch * 0.5) * sin(*yaw * 0.5);
  *q4 = cos(*roll * 0.5) * cos(*pitch * 0.5) * sin(*yaw * 0.5) - sin(*roll * 0.5) * sin(*pitch * 0.5) * cos(*yaw * 0.5);
}
