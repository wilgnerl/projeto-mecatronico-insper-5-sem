#ifndef MBED_MOTOR_H
#define MBED_MOTOR_H
 
#include "mbed.h"
 
class Motor{
    public:
        Motor(PinName pin);
        void gira_motor_sentido_horario(void);
        void gira_motor_sentido_antihorario(void);
    private:
        DigitalOut _pin;
}; 
 
#endif
