#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt

TOTAL_TIME = 15         # total time to plot (s)
MAX_ACCEL_TIME = 0.05   # time to maximum acceleration (s)
HOLD_TIME = 0.35        # time at maximum acceleation (s)
DECEL_TIME = 0.5        # time to decrease acceleration to g (s)
MAX_ACCEL = 160         # maximum acceleration (m/s^2)
G = -9.81               # acceleration of gravity (m/s^2)
DT = 0.0001             # time step in simulation (s)

ACC_STDEV = -0.09 * G   # accelerometer measurement standard dev (m/s^2)
ACC_STEP = -0.732 * G   # accelerometer measurement step size (m/s^2)
ACC_DT = 1.0/238.0      # accelerometer sampling period (s)

ALT_STDEV = 0           # altimeter measurement standard dev (m/s^2)
ALT_STEP = 0            # altimeter measurement step size (m/s^2)
ALT_DT = 0              # altimiter sampling period (s)


total_frames = int(TOTAL_TIME / DT)
t = np.linspace(0, TOTAL_TIME, total_frames)
a = np.zeros(total_frames)
v = np.zeros(total_frames)
x = np.zeros(total_frames)

max_accel_end = int(MAX_ACCEL_TIME / DT)
hold_end = max_accel_end + int(HOLD_TIME / DT)
decel_end = hold_end + int(DECEL_TIME / DT)

a[:max_accel_end] = np.linspace(G, MAX_ACCEL, max_accel_end)
a[max_accel_end:hold_end] = MAX_ACCEL
a[hold_end:decel_end] = np.linspace(MAX_ACCEL, G, decel_end - hold_end)
a[decel_end:] = G

for i in range(1, total_frames):
    x[i] = x[i - 1] + v[i - 1] * DT + 0.5 * a[i - 1] * DT ** 2
    if x[i] < 0:
        x[i] = 0

    v[i] = v[i - 1] + a[i - 1] * DT
    if v[i] < 0 and x[i] <= 0:
        v[i] = 0

acc_frames = int(TOTAL_TIME / ACC_DT)
acc_t = np.linspace(0, TOTAL_TIME, acc_frames)
acc_a_hold = np.interp(acc_t, t, a)
acc_a_noisy = acc_a_hold + np.random.normal(0, ACC_STDEV, acc_frames)
acc_a_digit = np.digitize(acc_a_noisy, np.arange(0, acc_a_noisy.max(), ACC_STEP))

alt_frames = int(TOTAL_TIME / ALT_DT)
alt_t = np.linspace(0, TOTAL_TIME, alt_frames)
alt_x_hold = np.interp(alt_t, t, x)
alt_x_noisy = acc_x_hold + np.random.normal(0, ACC_STDEV, alt_frames)
alt_x_digit = np.digitize(alt_x_noisy, np.arange(0, alt_x_noisy.max(), ALT_STEP))

plt.plot(acc_t, acc_a_digit)
plt.plot(alt_t, alt_x_digit)
plt.show()
