# config.py
from enum import IntEnum
import serial # For serial constants if needed

# --- MOTOR DEFINITIONS ---
class MotorIndex(IntEnum):
    EPU = 0
    EPD = 1
    EYR = 2
    EYL = 3
    WPU = 4
    WPD = 5
    RJL = 6
    LJR = 7
    LJL = 8
    RJR = 9
    ROLL = 10 

MOTOR_NAMES_FLAT = [
    "RJR", "RJL", "LJR", "LJL", "WPD", "WPU",
    "EYR", "EYL", "EPD", "EPU"
]
MOTOR_NAMES_GROUPED = [
    ["RJR", "RJL"], ["LJR", "LJL"], ["WPD", "WPU"],
    ["EYR", "EYL"], ["EPD", "EPU"]
]


# --- MOVEMENT PARAMETERS ---
STEP_SIZES = [10, 20, 50, 100, 300, 500, 900]
DEFAULT_STEP_SIZE_INDEX = STEP_SIZES.index(10)
MOTOR_TEST_DEFAULT_STEP_COUNT = 50
MOTOR_TEST_STEP_MIN = -200
MOTOR_TEST_STEP_MAX = 200

# --- SERIAL COMMUNICATION ---
DEFAULT_SERIAL_PORT = "COM3"
SERIAL_BAUDRATE = 9600
SERIAL_BYTESIZE = serial.EIGHTBITS
SERIAL_PARITY = serial.PARITY_NONE
SERIAL_STOPBITS = serial.STOPBITS_ONE
SERIAL_TIMEOUT = 1
SERIAL_CONNECT_DELAY = 2 # Time to wait for Arduino reset

# --- CONVERSION FACTORS ---
STEPS_TO_MM_LS = 200 / 0.3 # (steps_per_rev / mm_per_rev)

# --- DIRECTION CHANGE FACTORS (steps)

DIR_COMP = True
Q1_DR_COMP = 125 
Q2_DR_COMP = 103.58
Q3_DR_COMP = 132.3
Q4_L_DR_COMP = 271
Q4_R_DR_COMP = 558

# --- UI ---
APP_TITLE = "Elbow Control Simulator"
MAIN_WINDOW_GEOMETRY = "950x950"
TEST_MOTORS_WINDOW_GEOMETRY = "750x450"
