#include "imu.h"

// #define IMU 0x69  
#define MAG_AK09916 0x0C 
#define ACCEL_XOUT_H 0x2D  
#define GYRO_XOUT_H 0x33  
#define REG_PWR_MGMT_1 0x06   
#define REG_INT_PIN_CFG 0x0F  


imu::imu(String x, uint8_t channel, uint8_t addr) : name(x), channel(channel), addr(addr) {
    init();
}

void imu::init() {
    initializeICM20948(addr);
    initializeMagnetometer();
}

void imu::update() {
    // init();
    accelX = readRegister16(addr, ACCEL_XOUT_H);
    accelY = readRegister16(addr, ACCEL_XOUT_H + 2);
    accelZ = readRegister16(addr, ACCEL_XOUT_H + 4);
    gyroX = readRegister16(addr, GYRO_XOUT_H);
    gyroY = readRegister16(addr, GYRO_XOUT_H + 2);
    gyroZ = readRegister16(addr, GYRO_XOUT_H + 4);

    if (addr == 0x69) {
    Wire.beginTransmission(MAG_AK09916);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(MAG_AK09916, 1);
    uint8_t magStatus1 = Wire.read();

    if (magStatus1 & 0x01) {
        uint8_t magData[8];
        Wire.beginTransmission(MAG_AK09916);
        Wire.write(0x11);
        Wire.endTransmission();
        Wire.requestFrom(MAG_AK09916, 8);
        for (int i = 0; i < 8; i++) {
            magData[i] = Wire.read();
        }
        magX = (int16_t)(magData[1] << 8 | magData[0]);
        magY = (int16_t)(magData[3] << 8 | magData[2]);
        magZ = (int16_t)(magData[5] << 8 | magData[4]);
        uint8_t magStatus2 = magData[6];
        if (magStatus2 & 0x08) {
            init();
        }
    }} else {
        init();
        magX = magY = magZ = 0;
    }
}

float* imu::getData() {
    update();
    static float data[9];
    data[0] = accelX;
    data[1] = accelY;
    data[2] = accelZ;
    data[3] = gyroX;
    data[4] = gyroY;
    data[5] = gyroZ;
    data[6] = magX;
    data[7] = magY;
    data[8] = magZ;
    return data;
}

String imu::getDataString() {
    String dataString = name + ":";
    dataString += String(accelX) + ",";
    dataString += String(accelY) + ",";
    dataString += String(accelZ) + ",";
    dataString += String(gyroX) + ",";
    dataString += String(gyroY) + ",";
    dataString += String(gyroZ) + ",";
    dataString += String(magX) + ",";
    dataString += String(magY) + ",";
    dataString += String(magZ);
    return dataString;
}

void imu::initializeICM20948(uint8_t addr) {
    Wire.beginTransmission(addr);
    Wire.write(REG_PWR_MGMT_1);
    Wire.write(0x01); // Wake up the device
    Wire.endTransmission();
    delay(100);

    Wire.beginTransmission(addr);
    Wire.write(0x10); // ACCEL_CONFIG
    Wire.write(0x08); // Set accelerometer sensitivity to ±4g
    Wire.endTransmission();

    Wire.beginTransmission(addr);
    Wire.write(0x11); // GYRO_CONFIG
    Wire.write(0x08); // Set gyroscope sensitivity to ±500dps
    Wire.endTransmission();

    // disableI2CMasterMode(addr);

    Wire.beginTransmission(addr);
    Wire.write(REG_INT_PIN_CFG);
    Wire.write(0x02); // Enable I2C bypass mode
    Wire.endTransmission();


}

void imu::initializeMagnetometer() {
    Wire.beginTransmission(MAG_AK09916);
    Wire.write(0x31);
    Wire.write(0x08);
    Wire.endTransmission();
    delay(10);
}

int16_t imu::readRegister16(uint8_t addr, uint8_t reg) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(addr, 2);
    if (Wire.available() < 2) return 0;
    return (Wire.read() << 8) | Wire.read();
}

// void imu::disableI2CMasterMode(uint8_t addr) {
//     // Access the USER_CTRL register (0x03)
//     Wire.beginTransmission(addr);
//     Wire.write(0x03); // USER_CTRL register address
//     Wire.endTransmission(false);

//     // Read the current value of the USER_CTRL register
//     Wire.requestFrom(addr, 1);
//     uint8_t userCtrl = Wire.read();

//     // Clear the I2C_MST_EN bit (bit 5) to disable I2C master mode
//     userCtrl &= ~(1 << 5);

//     // Write the updated value back to the USER_CTRL register
//     Wire.beginTransmission(addr);
//     Wire.write(0x03); // USER_CTRL register address
//     Wire.write(userCtrl);
//     Wire.endTransmission();

//     // Optionally, you can also clear the I2C master configuration if needed
//     Wire.beginTransmission(addr);
//     Wire.write(0x24); // I2C_MST_CTRL register address
//     Wire.write(0x00); // Disable I2C master clock and other settings
//     Wire.endTransmission();
// }
