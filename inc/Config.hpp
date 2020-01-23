#pragma once

#include <math.h>
#include "CTD.h"
#include "motor.hpp"

const Speed g_fast_forward(300, 500, 100);

#define AXIS_MAIN CTD_AXIS_1
#define AXIS_X CTD_AXIS_4
#define AXIS_Z CTD_AXIS_3

const double DIVISION = 0.1;
const double PULSE_PER_MM_X = 1000 / 1.25;
const double PULSE_PER_MM_Z = 1000 / 2.5;

#define MM_PULSE_X(mm) ((mm)*PULSE_PER_MM_X)
#define MM_PULSE_Z(mm) ((mm)*PULSE_PER_MM_Z)
#define PULSE_MM_X(pulse) ((pulse) / PULSE_PER_MM_X)
#define PULSE_MM_Z(pulse) ((pulse) / PULSE_PER_MM_Z)

#define CALC_DISTANCE(p1, p2) (sqrt(pow((p2.m_x) - (p1.m_x), 2) + pow((p2.m_z) - (p2.m_z), 2)))