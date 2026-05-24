#include "I2Cdev.h"
#include "MPU6050.h"

#define TO_DEG 57.29577951308232087679815481410517033f

MPU6050 accel;

float angle_a_z, angle_a_x, angle_g_z, angle_g_x;
float total_angle_z = 0;
float total_angle_x = 0;
float dt;
long int t_next = 0;
float DispA = 0.018597219599999985;
float DispG = 0.00011221755000000218;
float Q = 0.001;
float P_z = 1.0;
float P_x = 1.0;

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
    dt = (t - t_next); // измерение прошедшего времени
    if(dt >= 20) {
        t_next += dt;
        int16_t ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw;
        float ax, ay, az, gx, gy, gz;

        accel.getMotion6(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);

        // нахождение углов через гловую скорость (a1 = a0 + ω * dt)
        gx = gx_raw / 131.0;
        gy = gy_raw / 131.0;
        gz = gz_raw / 131.0;

        angle_g_z = total_angle_z + gx * dt / 1000;
        P_z += Q;
        angle_g_x = total_angle_x + gy * dt / 1000;
        P_x += Q;

        // нахождение ускорений по осям +- 2g
        ax = ax_raw / 16384.0;
        ay = ay_raw / 16384.0;
        az = az_raw / 16384.0;

        // нахождение углов (крен, тангаж) через вектора силы тяжести
        angle_a_z = atan2(ay, az) * TO_DEG;

        angle_a_x = atan2(ax, az) * TO_DEG;

        float K_z = P_z / (P_z + DispA);
        total_angle_z = angle_g_z + K_z * (angle_a_z - angle_g_z);
        P_z = (1 - K_z) * P_z;

        float K_x = P_x / (P_x + DispA);
        total_angle_x = angle_g_x + K_x * (angle_a_x - angle_g_x);
        P_x = (1 - K_x) * P_x;

        Serial.print("Z: ");
        Serial.print(total_angle_z);
        Serial.print("  X: ");
        Serial.println(total_angle_x);
    }
}