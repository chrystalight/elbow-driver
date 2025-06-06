#include <AccelStepper.h>
#include "motors.h"


bool verbose_output = true;
bool test_mode_active = false;
int dir = 1; 
AccelStepper* current_test_motor = nullptr; // Will update to motors[0] below
AccelStepper* handleSelectMotor(String name);
void stepTest(int jointNum);


void setup() {  
    Serial.begin(9600);
    initializeMotors();
}

int handleTestMotorCommand(const String& command) {
    if (command == "START_TEST_MOTORS") {
        test_mode_active = true;
        current_test_motor = handleSelectMotor("EPU");
    } 
    else if (command.startsWith("SELECT_MOTOR:")) {
        String name = command.substring(13);
        current_test_motor = handleSelectMotor(name);
    } 
    else if (command == "FINE_TENSION") {
        tensionMotor(current_test_motor, false);
        Serial.println("FINE_TENSION_DONE");
    } 
    else if (command == "COARSE_TENSION") {
        tensionMotor(current_test_motor, true);
        Serial.println("COARSE_TENSION_DONE");
    } 
    else if (command == "DETENSION") {
        detensionMotor(current_test_motor);
        Serial.println("DETENSION_DONE");
    } 
    else if (command.startsWith("STEP_MOTOR_BY")) {
        String stepsStr = command.substring(14);
        int steps = stepsStr.toInt();
        Serial.print("STEPPING BY ");
        Serial.println(steps);
        current_test_motor->move(steps);
        while (current_test_motor->distanceToGo() != 0) {
            current_test_motor->run();
        }
        Serial.println("STEP_MOTOR_BY_DONE");
    }
    else if (command == "SET_HOME") {
        for (int i = 0; i < NUM_MOTORS; i++) {
            motors[i]->setCurrentPosition(0);
        }
        Serial.println("HOME_SET");
    }
    else if (command == "EXIT_TEST") {
        test_mode_active = false;
        Serial.println("TEST_MOTORS_EXITED");
    }
    else {
        return 0;
    }
    return 1;
}

AccelStepper* handleSelectMotor(String name){
    AccelStepper* motor = getMotorByName(name);
    if (motor != nullptr) {
        Serial.print("TEST_MOTOR_SELECTED:");
        Serial.println(name);
        return motor;
    } 
    Serial.print("ERROR: ATTEMPTED TO SELECT NULL MOTOR");
    return nullptr;
}

void stepTest(int jointNum){
    dir = dir * -1;
    int motor_pair[2] = {0, 0}; // Declare motor_pair as an array of 2 integers

    if (jointNum == 4) {
        //motor_pair[0] = 6;
        //motor_pair[1] = 9;
        motor_pair[0] = 7;
        motor_pair[1] = 8;

    } else if (jointNum == 3) {
        motor_pair[0] = 4;
        motor_pair[1] = 5; // Make sure there's a semicolon here
    }
    else if (jointNum == 2) {
        motor_pair[0] = 2;
        motor_pair[1] = 3; // Make sure there's a semicolon here
    }
    else if (jointNum == 1) {
        motor_pair[0] = 0;
        motor_pair[1] = 1; // Make sure there's a semicolon here
    }
        long startPosLJL = motors[motor_pair[0]]->currentPosition();
        long startPosLJR = motors[motor_pair[1]]->currentPosition();

        motors[motor_pair[0]]->setSpeed(dir*30);
        motors[motor_pair[1]]->setSpeed(dir*-30);

        Serial.println("START_TEST_Q_RUNNING");

        unsigned long startTime = millis();

        while (Serial.available() == 0) {
            motors[motor_pair[0]]->runSpeed();
            motors[motor_pair[1]]->runSpeed();
            delay(1);
        }

        unsigned long endTime = millis();
        long endPosLJL = motors[motor_pair[0]]->currentPosition();
        long endPosLJR = motors[motor_pair[1]]->currentPosition();

        motors[motor_pair[0]]->setSpeed(0);
        motors[motor_pair[1]]->setSpeed(0);

        long stepsLJL = endPosLJL - startPosLJL;
        long stepsLJR = endPosLJR - startPosLJR;
        unsigned long duration = endTime - startTime;

        Serial.print("TEST_Q_DONE;duration_ms:");
        Serial.print(duration);
        Serial.print(";steps_LJL:");
        Serial.print(stepsLJL);
        Serial.print(";steps_LJR:");
        Serial.println(stepsLJR);    
}

void handleMoveAllMotors(String cmd) {
    long motorSteps[NUM_MOTORS] = {0};
    cmd.trim();

    if (cmd.startsWith("MOVE_ALL_MOTORS:")) {
        String stepsData = cmd.substring(cmd.indexOf(':') + 1);
        char dataBuffer[stepsData.length() + 1];
        stepsData.toCharArray(dataBuffer, sizeof(dataBuffer));
        
        char* token = strtok(dataBuffer, ",");
        int motorIndex = 0;
        
        while (token != NULL && motorIndex < NUM_MOTORS) {
            motorSteps[motorIndex] = atol(token);
            motorIndex++;
            token = strtok(NULL, ",");
        }

        if (motorIndex == NUM_MOTORS) {
            if (verbose_output) {
                Serial.println("Moving motors with steps:");
                for (int i = 0; i < NUM_MOTORS; i++) {
                    Serial.print("Motor ");
                    Serial.print(getMotorName(i));
                    Serial.print(": ");
                    Serial.println(motorSteps[i]);
                }
            }

            // Command all motors to move
            for (int i = 0; i < NUM_MOTORS; i++) {
                if (motorSteps[i] != 0) {
                    motors[i]->move(motorSteps[i]);
                }
            }

            // Run all motors until movement is complete
            bool stillMoving = true;
            while (stillMoving) {
                stillMoving = false;
                for (int i = 0; i < NUM_MOTORS; i++) {
                    motors[i]->run();
                    if (motors[i]->distanceToGo() != 0) {
                        stillMoving = true;
                    }
                }
            }
            Serial.println("Motor movement commands applied.");
        } else {
            Serial.print("Error: Failed to parse all motor steps. Parsed ");
            Serial.print(motorIndex);
            Serial.print(" values, expected ");
            Serial.print(NUM_MOTORS);
            Serial.println();
        }
    }
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (test_mode_active || command == "START_TEST_MOTORS") {
            if(handleTestMotorCommand(command) == 1) {
                return;
            }
        }
        
        if (command.startsWith("MOVE_ALL_MOTORS:")) {
            handleMoveAllMotors(command);
        }
        else if (command == "TOGGLE_VERBOSE") {
            verbose_output = !verbose_output;
            Serial.print("VERBOSE_STATE:");
            Serial.println(verbose_output ? "1" : "0");
        }
        else if (command.startsWith("START_TEST_Q")){
            stepTest(1);
        }
        else {
            Serial.println("ERROR: Unknown command");
        }
    }
}
