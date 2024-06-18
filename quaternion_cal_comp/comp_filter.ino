// Usage:
// mpu4.getMotion6(&acc[0], &acc[1], &acc[2], &gyr[0], &gyr[1], &gyr[2]);
// getAngleFiltr(acc[0], acc[1], acc[2], gyr[0], gyr[1], gyr[2]);
// ToQuaternion(angle_fx, angle_fy, angle_fz, &q.w, &q.x, &q.y, &q.z); 

// put in main:
// void ToQuaternion(float roll, float pitch, float yaw, float *q1, float* q2, float* q3, float* q4) // roll (x), pitch (y), yaw (z), angles are in radians
// {
//     // Abbreviations for the various angular functions
//     float cr = cos(roll * 0.5);
//     float sr = sin(roll * 0.5);
//     float cp = cos(pitch * 0.5);
//     float sp = sin(pitch * 0.5);
//     float cy = cos(yaw * 0.5);
//     float sy = sin(yaw * 0.5);

//     Quaternion q0;
//     *q1 = cr * cp * cy + sr * sp * sy;
//     *q2 = sr * cp * cy - cr * sp * sy;
//     *q3 = cr * sp * cy + sr * cp * sy;
//     *q4 = cr * cp * sy - sr * sp * cy;
// }


#define FK 0.99                   // Complimentary filter coef
#define TO_DEG 57.2957f           // Radian


uint16_t* someArray[2];
int someValue = &someArray;

const uint16_t accelVal = 16384;                  //magic accel values
const uint8_t gyrVal = 131;                       //magic gyro values
double ax[8], ay[8], az[8], gx[8], gy[8], gz[8], angle_ax[8], angle_ay[8], angle_az[8]; //Filtered values
int16_t ax_raw[8], ay_raw[8], az_raw[8], gx_raw[8], gy_raw[8], gz_raw[8]; //Raw values

// Filter out noise
void GetAngleFiltr(int16_t* acc0, int16_t* acc1, int16_t* acc2, int16_t* gyr0, int16_t* gyr1, int16_t* gyr2, uint8_t accelNo) {
  // Assign to func variables
  AssignValues(*acc0, *acc1, *acc2, *gyr0, *gyr1, *gyr2, accelNo);
  // Get angles for this sensor
  // Getting "Measurement System" representation
  ax[accelNo] = ax_raw[accelNo] / accelVal;
  ay[accelNo] = ay_raw[accelNo] / accelVal;
  az[accelNo] = az_raw[accelNo] / accelVal;
    
  // Using trigonometry to turn it into angles
  angle_ax[accelNo] = TO_DEG * atan2(ay[accelNo], az[accelNo]);
  angle_ay[accelNo] = TO_DEG * atan2(ax[accelNo], az[accelNo]);
  angle_az[accelNo] = TO_DEG * atan2(ay[accelNo], ax[accelNo]);

  // Getting Angular speed
  gx[accelNo] = gx_raw[accelNo]/gyrVal;
  gy[accelNo] = gy_raw[accelNo]/gyrVal;
  gz[accelNo] = gz_raw[accelNo]/gyrVal;  

  // Integrating angular sped to get angle
  angle_fx[accelNo] += gx[accelNo] * TIME_GYRO/1000000.0 ;
  angle_fy[accelNo] += gy[accelNo] * TIME_GYRO/1000000.0 ;
  angle_fz[accelNo] += gz[accelNo] * TIME_GYRO/1000000.0 ;

  // Getting filtering using FK constant
  angle_fx[accelNo] = angle_fx[accelNo]*FK + angle_ax[accelNo]*(1-FK);
  angle_fy[accelNo] = angle_fy[accelNo]*FK + angle_ay[accelNo]*(1-FK);
  angle_fz[accelNo] = angle_fz[accelNo]*FK + angle_az[accelNo]*(1-FK);
}

void AssignValues(uint16_t acc0, uint16_t acc1, uint16_t acc2, uint16_t gyr0, uint16_t gyr1, uint16_t gyr2, uint8_t num){
    ax_raw[num] = acc0; ay_raw[num] = acc1; az_raw[num] = acc2; gx_raw[num] = gyr0; gy_raw[num] = gyr1; gz_raw[num] = gyr2;
}
