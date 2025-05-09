#ifndef IMU_H
#define IMU_H

#include <Wire.h>

class imu {

public:
    String name;
    uint8_t channel;
    uint8_t addr;
    
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    int16_t magX, magY, magZ;

    imu(String x, uint8_t channel, uint8_t addr);
    void update();
    float* getData();
    String getDataString();

private:
    void init();
    void initializeICM20948(uint8_t addr);
    void initializeMagnetometer();
    int16_t readRegister16(uint8_t addr, uint8_t reg);
    void disableI2CMasterMode(uint8_t addr);
};

#endif
