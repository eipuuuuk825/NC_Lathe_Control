#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "standard.hpp"
#include "motor.hpp"
#include "config.hpp"

std::string read_file(const char *file_path);
void analyze_gcode(const char *file_path);
bool is_tag(char tag);
void exe_analyze(std::string str);
std::vector<std::string> split_tag(std::string str_raw);
bool is_numeric(char input_char);
void print_q(void);
double find_previous_pos(size_t index, char X_or_Z);

/*-----------------------------------------------
*
* タグ
*
-----------------------------------------------*/
typedef void (*FuncPointer)(std::string str);
void analyze_G(std::string str);
void analyze_M(std::string str);
void analyze_S(std::string str);
void analyze_F(std::string str);

typedef struct
{
    char m_tag_char;
    FuncPointer m_tag_func;
} Tag;

const std::vector<Tag> g_tag{
    {'G', analyze_G},
    {'M', analyze_M},
    {'S', analyze_S},
    {'F', analyze_F},
};

/*-----------------------------------------------
*
* キュー
*
-----------------------------------------------*/
typedef enum
{
    MODE_CHANGE_SPEED_SPTEPPING_MOTOR, /* ステッピングモータ速度変更 */
    MODE_CHANGE_SPEED_SERVO,           /* サーボモータ速度変更 */
    MODE_DRIVE_STEPPING_MOTOR,         /* ステッピングモータ駆動 */
} QueueMode;

class Pos
{
public:
    double m_x, m_z;

public:
    Pos(void)
    {
        m_x = 0;
        m_z = 0;
    }
    Pos(double x, double z)
    {
        m_x = x;
        m_z = z;
    }
};

class Queue
{
public:
    Queue(QueueMode mode, Speed speed)
    {
        m_mode = mode;
        m_speed = speed;
    }
    Queue(QueueMode mode, Pos target_pos)
    {
        m_mode = mode;
        m_pos = target_pos;
    }

public:
    QueueMode m_mode; /* キューのモード */
    Speed m_speed;    /* 目標速度 */
    Pos m_pos;        /* 目標位置 */
};