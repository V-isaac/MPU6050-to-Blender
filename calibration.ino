#include <Wire.h>
#include "MPU6050_6Axis_MotionApps612.h"

// упрощеный I2C адрес нашего гироскопа/акселерометра MPU-6050.
MPU6050 mpu(0x68);
 
void MuxSelect(uint8_t i);

// Allocating memory to rewrite each time
uint8_t fifoBuffer[64];

void setup() {

  //Wire.setWireTimeout(250000, true);
  Serial.begin(115200); // Запуск общения по USB

  /* Enable I2C */
  Wire.setClock(200000); // Запуск общения по IIC
  Wire.begin();//(22,23); //SDA //SCL
  Serial.println("");

  uint8_t error_code = 0U, errors = 0; // Локальные переменные для фиксации ошибок

  for (uint8_t it = 0; it <= 7; it++) {
    MuxSelect(it); // Выбор адреса
    Serial.print("Инициализируем MPU#"); Serial.print(it); Serial.println("");
    //Initialisation and bunch of tests
    mpu.initialize(); // Инициализация датчика через библеотеку
    error_code = mpu.dmpInitialize(); // Инициализация DMP. Функция возвращает код ошибки при наличии
    if (error_code == 1U) {
      Serial.print("Устройство "); Serial.print(it); Serial.println(" инициализация неудалась: неудача в первычной загрузке памяти.");
      errors++;
    }
    if (error_code == 2U) {
      Serial.print("Device "); Serial.print(it); Serial.println(" инициализация неудалась: неудача конфигурации обновлений DMP.");
      errors++;
    }
    if(!mpu.testConnection()){ Serial.println("Неудалось установить связь с датчиком."); }

    mpu.setDMPEnabled(true); // Включаем DMP при удачном запуске
  }

  if (errors != 0) {
    Serial.print("Количество ошибок: "); Serial.print(errors); 
    while (1) {}
  }
  
  Serial.println("Отлично! Всё готово к работе.");
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
 
//Функция мультиплексирует адреса для обмена данными
void MuxSelect(uint8_t i) {
  if (i > 7) return; // Если выходим за пределы адресов - не взаимодействуем
 
  Wire.beginTransmission(0x70); // Начинаем общение по данному адресу
  Wire.write(1 << i); // Сместить Истину на определённый регистр
  Wire.endTransmission(); // Закончить общение
}
