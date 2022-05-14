/*
  Lab1 mbed Hands-on Lesson
  「衛星が軌道を周回する様子を再現しよう」
*/

#include "mbed.h"
#include <cstdio>

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
            isSunlit = theta < 180.0;
            printf("%.0f deg, %s, %s\r\n", theta, isInVisibleArea ? "Visible" : "NotVisible", isSunlit ? "Sunlit" : "Shaded");
        }
};

class Satellite {
    private:
        Timer timer;

    public:
        Satellite() {
            timer.start();
        }
        
        bool isConnected;

        float time(){
            return timer.read();
        }

        void checkConnection(Environment environment) {
            isConnected = environment.isInVisibleArea;
        }
};

DigitalOut conditions[] = {LED1, LED2, LED3, LED4};
    
int main(){
    Satellite sat;
    Environment environment;

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

        if(sat.isConnected){
            // 可視範囲内
            conditions[1] = 1;
        }else{
            // 可視範囲外
            conditions[1] = 0;
        }

        printf("time: %.2f, %s\r\n", sat.time(), sat.isConnected ? "Visible" : "NotVisible");
        wait(0.2);
    }
}
