#include "MPU9250.h"

MPU9250 mpu;

double previousXValue = 0;
double previousYValue = 0;
double previousZValue = 0;

void setup()
{
    Serial.begin(115200);

    Wire.begin();

    delay(200);
    mpu.setup();
}

void loop()
{
    static uint32_t prev_ms = millis();
    if ((millis() - prev_ms) > 16)
    {
        mpu.update();
        mpu.print();

        double currentXValue = mpu.getRoll();
        double currentYValue = mpu.getPitch();
        double currentZValue = mpu.getYaw();
        if( abs(previousXValue - currentXValue) > (0.2*abs(previousXValue)))
        {
          Serial.print("roll  (x-forward (north)) : ");
          Serial.println(currentXValue);
          previousXValue = currentXValue;
        }

        if(abs(previousYValue - currentYValue) > (0.2*abs(previousYValue)))
        {
          Serial.print("pitch (y-right (east))    : ");
          Serial.println(currentYValue);
          previousYValue = currentYValue;
        }

        if(abs(previousZValue - currentZValue) > (0.2*abs(previousZValue)))
        {
          Serial.print("yaw   (z-down (down))     : ");
          Serial.println(currentZValue);
          previousZValue = currentZValue;
        }
        
        delay(20);
        prev_ms = millis();
    }
}
