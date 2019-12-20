#include <cstdio>
#include <vector>
#include <fstream>
#include "../inc/motor.hpp"
#include "../inc/g_code.hpp"

extern std::vector<Queue> g_q;
Motor g_motor(0, 4);

void wait_busy(void)
{
	while (g_motor.get_busy(AXIS_X) | g_motor.get_busy(AXIS_Z))
	{
		if (GetAsyncKeyState('Q'))
		{
			printf("*** slow down stop ***\n");
			g_motor.slow_down_stop(AXIS_X);
			g_motor.slow_down_stop(AXIS_Z);
			exit(EXIT_SUCCESS);
		}
	}
}

int main(void)
{
	/*-----------------------------------------------
	*
	* G コードを解析
	*
	-----------------------------------------------*/
	analyze_gcode("Gcode_sample.txt");
	print_q();

	/*-----------------------------------------------
	*
	* コントローラボード準備
	*
	-----------------------------------------------*/
	/* 初期化 */
	g_motor.change_speed(AXIS_Z, Speed(300, 1000, 100));
	/* 原点合わせ */
	g_motor.drive_limit_switch(AXIS_Z, MINUS);
	wait_busy();

	g_motor.set_int_cnt(AXIS_X, 0);
	g_motor.set_int_cnt(AXIS_Z, 0);

	/*-----------------------------------------------
	*
	* キューの処理を実行
	*	
	-----------------------------------------------*/
	for (size_t i = 0; i < g_q.size(); i++)
	{
		switch (g_q[i].m_mode)
		{
		/* ステッピングモータ速度変更 */
		case MODE_CHANGE_SPEED_SPTEPPING_MOTOR:
			g_motor.change_speed(AXIS_X, g_q[i].m_speed);
			g_motor.change_speed(AXIS_Z, g_q[i].m_speed);
			printf("%4d/%4d\tspeed %.0lf\n", i + 1, g_q.size(), g_q[i].m_speed.m_object);
			wait_busy();
			break;
		/* ステッピングモータ駆動 */
		case MODE_DRIVE_STEPPING_MOTOR:
			g_motor.drive_int_cnt(AXIS_X, MM_PULSE_X(g_q[i].m_pos.m_x));
			g_motor.drive_int_cnt(AXIS_Z, MM_PULSE_Z(g_q[i].m_pos.m_z));
			printf("%4d/%4d\tpos (%.2lf, %.2lf)\n",
				   i + 1, g_q.size(),
				   g_q[i].m_pos.m_x, g_q[i].m_pos.m_z);
			wait_busy();
			break;
		}
	}

	return 0;
}