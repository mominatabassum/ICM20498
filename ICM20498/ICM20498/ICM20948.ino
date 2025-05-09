#include "imu.h"
#include "BluetoothSerial.h"
#include "TCA9548A.h"

TCA9548A mux;

BluetoothSerial SerialBT;
#define SDA_PIN 21
#define SCL_PIN 22

imu sensors[] = {
    imu("r_shoulder", 3, 0x69),
    imu("r_elbow", 3, 0x68)
}; 
size_t num = sizeof(sensors) / sizeof(sensors[0]);


void setup() {
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);
    
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(200000);
    Serial.begin(250000);
    delay(1000);
    // SerialBT.begin("MoCap");
    mux.begin(Wire);             // Wire instance is passed to the library
    mux.closeAll(); 
}


uint8_t success = 0; 

void loop() {
    String data = "";

    if (success == 100) {
            success == 0;
            for (int i = 0; i < num; i++) {
                mux.openChannel(sensors[i].channel);
                sensors[i].update();
                data += sensors[i].getDataString();
                data += "\n";
            }

    } else {
            for (int i = 0; i < num; i++) {
                mux.openChannel(sensors[i].channel);
                data += sensors[i].getDataString();
                data += "\n";
            }

    }



    data += "sent";
    Serial.println(data); 
    // SerialBT.println(data); 

    //delay(500);
    success++;
}

