#include <Arduino.h>
#include "modules/signal_receiver.h"
#include "modules/servo_controller.h"
#include "modules/motor_controller.h"
#include "modules/flight_controller.h"

// Сначала создаем зависимости
SignalReceiver signalReceiver;
ServoController servoController;
MotorController motorController;

// Затем создаем FlightController, передавая зависимости
FlightController flightController(servoController, motorController);

void setup() {
    Serial.begin(115200);
    
    signalReceiver.init();
    servoController.init();
    motorController.init();
    flightController.init();
    
    Serial.println("Самолет запущен!");
}

void loop() {
    ControlSignal signal = signalReceiver.getSignal();
    flightController.processSignal(signal);
    delay(10);
}