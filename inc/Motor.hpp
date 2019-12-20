#pragma once

#include <windows.h>
#include <cstdio>
#include <conio.h> // kbhit
#include <stdint.h>
#include "CTDw.h"

typedef enum
{
    PLUS,
    MINUS,
} DIRECTION;

class Speed
{
public:
    double m_ss, m_object, m_rate;
    Speed(void)
    {
        m_ss = 0;
        m_object = 0;
        m_rate = 0;
    }
    Speed(double ss, double object, double rate)
    {
        m_ss = ss;
        m_object = object;
        m_rate = rate;
    }
    inline bool operator==(const Speed &rhs) const
    {
        bool _1 = (m_ss == rhs.m_ss);
        bool _2 = (m_object == rhs.m_object);
        bool _3 = (m_rate == rhs.m_rate);
        return (_1 & _2 & _3);
    }
    inline bool operator!=(const Speed &rhs) const
    {
        return !(*this == rhs);
    }
};

class Motor
{
public:
    void change_speed(uint16_t axis, Speed speed);
    void drive_limit_switch(uint16_t axis, DIRECTION direction);
    void drive_int_cnt(uint16_t axis, int32_t target_cnt);
    void slow_down_stop(uint16_t axis);

public:
    Motor(uint16_t bsn, size_t max_axis_num);
    ~Motor(void);
    bool get_busy(uint16_t axis);
    int32_t get_int_cnt(uint16_t axis);
    void set_int_cnt(uint16_t axis, int32_t int_cnt);

private:
    uint16_t m_bsn;
    void init_axis(uint16_t axis);
    void preset_pulse_drive(uint16_t axis, int64_t pulse);

private:
    Speed m_pre_speed;
};