#include "I2Cdev.h"
#include "MPU6050.h"  // Using basic MPU6050 library, not MotionApps

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

// Calibration offsets
int16_t ax_offset = 0, ay_offset = 0, az_offset = 0;
int16_t gx_offset = 0, gy_offset = 0, gz_offset = 0;

void setup() {
    Serial.begin(115200);
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(100000);
    #endif
    
    Serial.println("Initializing MPU6050...");
    mpu.initialize();
    
    Serial.println("Testing device connections...");
    Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    
    Serial.println("\n=== MPU6050 CALIBRATION ===");
    Serial.println("Place the sensor on a LEVEL surface and DO NOT MOVE it!");
    Serial.println("Calibration will start in 5 seconds...");
    delay(5000);
    
    calibrateSensor();
    
    Serial.println("\n=== CALIBRATION COMPLETE ===");
    Serial.println("Add these values to your main code:");
    Serial.println("-----------------------------------");
    Serial.print("mpu.setXGyroOffset(");
    Serial.print(gx_offset);
    Serial.println(");");
    
    Serial.print("mpu.setYGyroOffset(");
    Serial.print(gy_offset);
    Serial.println(");");
    
    Serial.print("mpu.setZGyroOffset(");
    Serial.print(gz_offset);
    Serial.println(");");
    
    Serial.print("mpu.setXAccelOffset(");
    Serial.print(ax_offset);
    Serial.println(");");
    
    Serial.print("mpu.setYAccelOffset(");
    Serial.print(ay_offset);
    Serial.println(");");
    
    Serial.print("mpu.setZAccelOffset(");
    Serial.print(az_offset -16384);  // Adjust for 1g gravity
    Serial.println(");  // Note: Z-axis adjusted for gravity");
    
    Serial.println("-----------------------------------");
}

void loop() {
    // Display current readings with offsets applied
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    Serial.print("ACCEL (with offsets): ");
    Serial.print(ax); Serial.print(", ");
    Serial.print(ay); Serial.print(", ");
    Serial.print(az); Serial.print(" | GYRO: ");
    Serial.print(gx); Serial.print(", ");
    Serial.print(gy); Serial.print(", ");
    Serial.println(gz);
    
    delay(500);
}

void calibrateSensor() {
    const int samples = 1000;
    long ax_sum = 0, ay_sum = 0, az_sum = 0;
    long gx_sum = 0, gy_sum = 0, gz_sum = 0;
    
    Serial.println("Calibrating... 0%");
    
    for (int i = 0; i < samples; i++) {
        mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        
        ax_sum += ax;
        ay_sum += ay;
        az_sum += az;
        gx_sum += gx;
        gy_sum += gy;
        gz_sum += gz;
        
        if (i % 100 == 0) {
            Serial.print("\rCalibrating... ");
            Serial.print((i * 100) / samples);
            Serial.print("%");
        }
        delay(2);
    }
   
    // Calculate averages
    //ax_offset = ax_sum / samples;
    //ay_offset = ay_sum / samples;
    //az_offset = az_sum / samples;
    //gx_offset = gx_sum / samples;
    //gy_offset = gy_sum / samples;
    //gz_offset = gz_sum / samples;
    

    ax_offset = -(ax_sum / samples);
    ay_offset = -(ay_sum / samples);
    az_offset = 16384-(az_sum / samples) ;  // compensate gravity

    gx_offset = -(gx_sum / samples);
    gy_offset = -(gy_sum / samples);
    gz_offset = -(gz_sum / samples);
    Serial.println("\rCalibrating... 100% Done!");
    
    // Apply offsets
    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);
    
    Serial.println("Offsets applied to sensor!");
}