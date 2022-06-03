/*
 * mbed Hands-on Lesson
 * ・Lab1: Let's simulate the satellite in orbit!
 * ・Lab2: Let's add EPS Subsystem functions.
 * ・Lab3: Let's add CDH Subsystem functions.
*/

#include "mbed.h"
#include "LocalFileSystem.h"
#include <cstdio>

class Environment {
    private:
        void updatePosition() {
            theta += 1.0; 
        }

    public:
        Environment() {
            theta = 0.0;
            isInVisibleArea = false;
            isSunlit = false;
        }

        float theta;
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

        FILE *fp;
        bool isFirstWriting;

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
            isFirstWriting = true;
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

        void setPowerSavingMode() {
            if(currentBatteryVoltage < 3.5){
                isPowerSavingMode = true;
            } else if(currentBatteryVoltage > 4.0) {
                isPowerSavingMode = false;
            }
        }
        
        void openFile(const char *filename) {
            fp = fopen(filename, "w");
        }
        
        void saveData(char *header, float *dataArray, int dataNum) {
            if (isFirstWriting) {
                fprintf(fp, "%s\r\n", header);
                isFirstWriting = false;
            }
            for(int i = 0; i < dataNum; i++) {
                fprintf(fp, "%.2f,", dataArray[i]);
            }
            fprintf(fp, "\r\n");
        }

        void closeFile() {
            fclose(fp);
        }
};

DigitalOut conditions[] = {LED1, LED2, LED3, LED4};
LocalFileSystem local("local");
    
int main(){
    Satellite sat;
    Environment environment;

    float batteryVoltage;

    // Turn on satellite. 
    printf("Released!\r\n");
    conditions[0] = 1;
    sat.openFile("/local/logdata.csv");
    
    // Wait a minute not to disturb main satellite
    while(sat.time() < 5.0) {
        printf("waiting...\r\n");
        wait(1.0);
    }

    // Start Nominal Operation
    printf("Start Operation\r\n");
    while(environment.theta < 360) {
        environment.update();

        sat.checkConnection(environment);
        sat.getBatteryVoltage(environment);
        sat.setPowerSavingMode();

        if(sat.isConnected){
            // Processing to be executed within the visible range
            conditions[1] = 1;
        }else{
            // Processing to be executed outside the visible range
            conditions[1] = 0;
        }

        char header[] = "time, battery";
        float data[] = {sat.time(), sat.currentBatteryVoltage};
        int numberOfData = sizeof(data) / sizeof(float);
        sat.saveData(header, data, numberOfData);
        
        printf("time: %.2f, %s, ", sat.time(), sat.isConnected ? "Visible" : "NotVisible");
        printf("battery: %.2f, mode: %s", sat.currentBatteryVoltage, sat.isPowerSavingMode ? "PowerSaving" : "Normal");
        printf("\r\n");
        wait(0.2);
    }
    
    printf("End Operation\r\n");
    sat.closeFile();
    
}
