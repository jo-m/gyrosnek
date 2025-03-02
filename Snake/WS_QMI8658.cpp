#include "WS_QMI8658.h"

#define I2C_SDA 11
#define I2C_SCL 12

SensorQMI8658 QMI;
IMUdata Accel;

void QMI8658_Init() {
  Wire.begin(I2C_SDA, I2C_SCL);
  QMI.begin(Wire, QMI8658_L_SLAVE_ADDRESS, I2C_SDA, I2C_SCL);

  QMI.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                          SensorQMI8658::ACC_ODR_1000Hz,
                          SensorQMI8658::LPF_MODE_0);
  QMI.configGyroscope(SensorQMI8658::GYR_RANGE_64DPS,
                      SensorQMI8658::GYR_ODR_896_8Hz, SensorQMI8658::LPF_MODE_3

  );

  // In 6DOF mode (accelerometer and gyroscope are both enabled),
  // the output data rate is derived from the nature frequency of gyroscope
  QMI.enableGyroscope();
  QMI.enableAccelerometer();
}

void QMI8658_Loop() {
  if (QMI.getDataReady()) {
    if (QMI.getAccelerometer(Accel.x, Accel.y, Accel.z)) {
      printf("x=%f y=%f z=%f\n", Accel.x, Accel.y, Accel.z);
    }
  }
}
