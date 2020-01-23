#include <cstdio>
#include <vector>
#include <fstream>
#include "../inc/motor.hpp"
#include "../inc/g_code.hpp"

#define BIT(x) (1 << (x))

extern std::vector<Queue> g_q;
Motor g_motor(0, 4);

void wait_busy(size_t axes)
{
	while (1)
	{
		/* ビジーフラグを確認 */
		bool is_busy = false;
		for (size_t i = 0; i < g_motor.m_max_axis_num; ++i)
		{
			if ((axes >> i) & 1)
				is_busy |= g_motor.get_busy(i);
		}

		/* ビジーフラグがすべて下がっていたらループを抜ける */
		if (!is_busy)
			break;

		/* キー入力でモータを停止 */
		if (GetAsyncKeyState('Q'))
		{
			printf("*** slow down stop ***\n");
			for (size_t i = 0; i < g_motor.m_max_axis_num; ++i)
				g_motor.slow_down_stop(i);
			exit(EXIT_SUCCESS);
		}
	}
}

#if 1
int main(void)
{
	/*-----------------------------------------------
	*
	* G コードを解析
	*
	-----------------------------------------------*/
	// analyze_gcode("Gcode_sample.txt");
	analyze_gcode("Gcode_sample_short.txt");
	print_q();

	/*-----------------------------------------------
	*
	* コントローラボード準備
	*
	-----------------------------------------------*/
	/* 原点合わせ */
	// g_motor.drive_limit_switch(AXIS_Z, MINUS);
	// wait_busy();

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
			printf("%4d/%4d\tstepping motor speed %.0lf\n", i + 1, g_q.size(), g_q[i].m_speed.m_object);
			wait_busy(BIT(AXIS_X) | BIT(AXIS_Z));
			break;
		/* ステッピングモータ駆動 */
		case MODE_DRIVE_STEPPING_MOTOR:
			g_motor.drive_int_cnt(AXIS_X, MM_PULSE_X(g_q[i].m_pos.m_x));
			g_motor.drive_int_cnt(AXIS_Z, MM_PULSE_Z(g_q[i].m_pos.m_z));
			printf("%4d/%4d\tpos (%.2lf, %.2lf)\n",
				   i + 1, g_q.size(),
				   g_q[i].m_pos.m_x, g_q[i].m_pos.m_z);
			wait_busy(BIT(AXIS_X) | BIT(AXIS_Z));
			break;
		/* 主軸駆動 */
		case MODE_DRIVE_SERVO:
			g_motor.drive_main_axis(AXIS_MAIN);
			printf("%4d/%4d\tdrive servo\n",
				   i + 1, g_q.size());
			break;
		/* 主軸停止 */
		case MODE_STOP_SERVO:
			g_motor.slow_down_stop(AXIS_MAIN);
			printf("%4d/%4d\tstop servo\n",
				   i + 1, g_q.size());
			break;
		/* 主軸速度変更 */
		case MODE_CHANGE_SPEED_SERVO:
			g_motor.change_speed(AXIS_MAIN, g_q[i].m_speed);
			printf("%4d/%4d\tservo speed\n",
				   i + 1, g_q.size());
			break;
		}
	}

	return 0;
}
#else
int main(void)
{
	BYTE get;

	CTDwUniversalSignalWrite(0, AXIS_MAIN, 0b00000011);
	//CTDwGetUniversalSignal(0, AXIS_MAIN, &get);
	//printf("%d\n", get);

	g_motor.change_speed(AXIS_MAIN, Speed(100, 10000, 100));
	// 	g_motor.set_int_cnt(AXIS_MAIN, 0);
	// g_motor.drive_int_cnt(AXIS_MAIN, 50000);
	// CTDwDataFullWrite(0, AXIS_MAIN, CTD_MINUS_PRESET_PULSE_DRIVE, 10000);
	//CTDwDataFullWrite(0, AXIS_MAIN, CTD_PLUS_CONTINUOUS_DRIVE, 0);
	g_motor.drive_main_axis(AXIS_MAIN);
	//CTDwDataFullWrite(0, AXIS_MAIN,  CTD_SLOW_DOWN_STOP, 0);

	wait_busy();

	//CTDwDataFullWrite(0, AXIS_MAIN,  CTD_SLOW_DOWN_STOP, 0);
}
#endif