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

class SPEED
{
public:
    double m_SS, m_Object, m_Rate;
    SPEED(void);
    SPEED(double SS, double Object, double Rate)
    {
        m_SS = SS;
        m_Object = Object;
        m_Rate = Rate;
    }
};

class MOTOR
{
public:
    MOTOR(uint16_t Bsn, size_t MaxAxisNum);
    ~MOTOR(void);
    void ChangeSpeed(uint16_t Axis, SPEED Speed);
    bool GetBusy(uint16_t Axis);
    int32_t GetIntCnt(uint16_t Axis);
    void SetIntCnt(uint16_t Axis, int32_t IntCnt);
    void DriveLimitSwitch(uint16_t Axis, DIRECTION Direction);
    void DriveIntCnt(uint16_t Axis, int32_t TargetCnt);

private:
    uint16_t m_Bsn;
    void InitAxis(uint16_t Axis);
    void PresetPulseDrive(uint16_t Axis, int64_t Pulse);
};