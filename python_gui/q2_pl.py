##ELBOW YAW MATH
##VERSION: 1.15


##LIMITATIONS: NEUTRAL +- 50 DEGREES!
##THETA ABODE 140 OR BELOW 40 IS NOT VALID!

import math
import numpy as np 
import config as cf
from config import MotorIndex, STEPS_TO_MM_LS
import matplotlib.pyplot as plt

dir_offset = cf.Q2_DR_COMP


EY_effective_radius = 1.3 ##mm

def get_jaw_pl(delta_theta):
    r2 = 1.3
    delta_s = math.radians(delta_theta)*r2

    return delta_s


def get_steps(curr_theta, delta_theta, latest_dir):

    
    motor_steps = [0] * len(cf.MotorIndex)  
    if delta_theta == 0:
        return motor_steps, latest_dir
    #curr theta is in degrees between 0 and 180

    delta_ey = math.radians(delta_theta) * EY_effective_radius
    steps_ey = int(delta_ey*(cf.STEPS_TO_MM_LS))

    if (latest_dir == 0 or latest_dir*delta_theta < 0 and cf.DIR_COMP):
        ###latest_dir and delta_theta are not the same direction/sign
        ###have to add compensation and swap direction
        comp = math.copysign(dir_offset, steps_ey)
        if(latest_dir == 0):
            comp = comp/2
        steps_ey += comp
        latest_dir = delta_theta
        print(f"Added {comp} steps due to a change in direction! new latest dir: {latest_dir}")
    else:

        print(f"latest_dir didnt change or dir_comp is off: latest_dir = {latest_dir} delta theta = {delta_theta} and dr comp = {cf.DIR_COMP}")
        print("oopsies")

    ## when I shorten yaw left, i have to shorten left jaw left and right jaw left

    motor_steps[cf.MotorIndex.EYR] = -steps_ey
    motor_steps[cf.MotorIndex.EYL] = steps_ey

    steps_q4 = int(get_jaw_pl(delta_theta)*STEPS_TO_MM_LS)

    motor_steps[cf.MotorIndex.RJL] = steps_q4
    motor_steps[cf.MotorIndex.LJL] = steps_q4
    motor_steps[cf.MotorIndex.RJR] = -steps_q4
    motor_steps[cf.MotorIndex.LJR] = -steps_q4

    return motor_steps, latest_dir


def plot_aux_cable_lengths():
    q2_range = np.linspace(40, 140, 361)
    jl_lengths = []
    jr_lengths = []

    for theta in q2_range:
        pl_jl, pl_jr = get_aux_pl(theta)
        jl_lengths.append(pl_jl)
        jr_lengths.append(pl_jr)

    plt.figure(figsize=(8, 5))
    plt.plot(q2_range, jl_lengths, label='Jaw Left Cable Length')
    plt.plot(q2_range, jr_lengths, label='Jaw Right Cable Length')
    plt.xlabel('q2 (degrees)')
    plt.ylabel('Cable Length (mm)')
    plt.title('Aux Cable Lengths vs q2')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('aux_cable_lengths_q2.png')
    plt.show()
