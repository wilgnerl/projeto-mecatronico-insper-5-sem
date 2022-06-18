#include "Motor.h"
#include "mbed.h"

Motor::Motor(PinName pin) : _pin(pin) {
    _pin = 0;
}
    

void Motor::gira_motor_sentido_horario(void){
    _pin = false;
}

void Motor::gira_motor_sentido_antihorario(void){
    _pin = true;
}
