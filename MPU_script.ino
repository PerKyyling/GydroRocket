#include "I2Cdev.h"
#include "MPU6050.h"

#define TO_DEG 57.29577951308232087679815481410517033f
#define T_OUT 20

MPU6050 accel;

float angle_x, angle_y, angle_z;
long int t_next;

float clamp(float v, float minv, float maxv){
    if( v>maxv )
        return maxv;
    else if( v<minv )
        return minv;
    return v;
}

void setup() {
    Serial.begin(9600);
    Wire.begin();

    accel.reset();
    delay(100);

    accel.initialize();
    accel.setSleepEnabled(false);

    Serial.println(accel.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}


void loop() {
    long int t = millis();
    if( t_next < t ){
        int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
        float ax, ay, az;

        t_next = t + T_OUT;
        accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);

        ax = ax_raw / 16384.0;
        ay = ay_raw / 16384.0;
        az = az_raw / 16384.0;

        float norm = sqrt(ax*ax + ay*ay + az*az);
        if(norm > 0.1) {
            ax /= norm;
            ay /= norm;
            az /= norm;
        }


        angle_x = atan2(ay, az) * TO_DEG;

        angle_y = atan2(ax, az) * TO_DEG;


        Serial.print("Z: ");
        Serial.print(angle_x);
        Serial.print("  X: ");
        Serial.println(angle_y);


    }
}