/*
 * mbed Hands-on Lesson
 * ・Lab1: Let's simulate the satellite in orbit!
 * ・Lab2: Let's add EPS Subsystem functions.
*/

#include "mbed.h"

class Environment {
    private:
        float theta;

        void updatePosition() {
            theta += 1.0; 
            if (theta > 360.0) {
                theta = 0.0;
            }
        }

    public:
        Environment() {
            theta = 0.0;
            isInVisibleArea = false;
            isSunlit = false;
        }

        bool isInVisibleArea;
        bool isSunlit;

        void update() {
            updatePosition();
            isInVisibleArea = (theta >= 70.0 && theta <= 110.0) || (theta >= 250.0 && theta <= 290.0);
            isSunlit = theta > 180.0;
            printf("%.0f deg, %s, || ", theta, isSunlit ? "Sunlit" : "Shaded");
        }
};

class Satellite {
    private:
        Timer timer;
        float batteryVoltage;
        float chargingCurrent;
        float consumptionCurrent;
        

        void calculateBatteryVoltage(Environment environment) {
            chargingCurrent = environment.isSunlit ? 0.008 : 0.0;
            consumptionCurrent = isPowerSavingMode ? 0.002 : 0.005;
            
            batteryVoltage += chargingCurrent - consumptionCurrent;

            if (batteryVoltage > 4.2) {
                batteryVoltage = 4.2;
            }
        }

    public:
        Satellite() {
            timer.start();
            batteryVoltage = 4.2;
            chargingCurrent = 0.0;
            consumptionCurrent = 0.0;
            isPowerSavingMode = false;
        }
        
        bool isConnected; /* Wheter the satellite is in the visible range. */

        float currentBatteryVoltage;
        bool isPowerSavingMode; /* Wheter the satellite state is in power-saving mode. */

        float time(){
            return timer.read();
        }

        void checkConnection(Environment environment) {
            isConnected = environment.isInVisibleArea;
        }

        void getBatteryVoltage(Environment environment) {
            calculateBatteryVoltage(environment);
            currentBatteryVoltage = batteryVoltage;
        }

        void setPowerSavingMode(Environment environment) {
            if(currentBatteryVoltage < 3.5){
                isPowerSavingMode = true;
            } else if(currentBatteryVoltage > 4.0) {
                isPowerSavingMode = false;
            }
        }
};

DigitalOut conditions[] = {LED1, LED2, LED3, LED4};
    
int main(){
    Satellite sat;
    Environment environment;

    float batteryVoltage;

    // Turn on satellite. 
    printf("Released!\r\n");
    conditions[0] = 1;
    
    // Wait a minute not to disturb main satellite
    while(sat.time() < 5.0) {
        printf("waiting...\r\n");
        wait(1.0);
    }

    // Start Nominal Operation
    while(1) {
        environment.update();

        sat.checkConnection(environment);

        sat.getBatteryVoltage(environment);
        sat.setPowerSavingMode(environment);

        if(sat.isConnected){
            // Processing to be executed within the visible range
            conditions[1] = 1;
        }else{
            // Processing to be executed outside the visible range
            conditions[1] = 0;
        }

        printf("time: %.2f, %s, ", sat.time(), sat.isConnected ? "Visible" : "NotVisible");
        printf("battery: %.2f, mode: %s", sat.currentBatteryVoltage, sat.isPowerSavingMode ? "PowerSaving" : "Normal");
        printf("\r\n");
        wait(0.2);
    }
}
