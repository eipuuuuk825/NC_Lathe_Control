#include "../inc/motor.hpp"
#include "../inc/standard.hpp"

/*-----------------------------------------------
*
* 初期化処理
*
-----------------------------------------------*/
Motor::Motor(uint16_t bsn, size_t max_axis_num)
{
	m_bsn = bsn;	   /* メンバの設定 */
	CTDwDllOpen();	 /* CTD.Dll を開く */
	CTDwCreate(m_bsn); /* デバイスの使用を宣言する */

	for (size_t i = 0; i < max_axis_num; i++)
	{
		init_axis(i);	  /* 制御軸の初期化 */
		set_int_cnt(i, 0); /* 内部カウンタ初期化 */
	}
}

/*-----------------------------------------------
*
* 終了処理
*
-----------------------------------------------*/
Motor::~Motor(void)
{
	CTDwClose(0);   /* デバイスを解放する */
	CTDwDllClose(); /* CTD.Dll を閉じる */
}

/*-----------------------------------------------
*
* 制御軸の初期化
*
-----------------------------------------------*/
void Motor::init_axis(uint16_t axis)
{
	// MODE1 SET
	CTDwMode1Write(m_bsn, axis, 0b01001001);
	// MODE2 SET
	CTDwMode2Write(m_bsn, axis, 0x3F);
	// モード設定
	CTDwCommandWrite(m_bsn, axis, CTD_INPOSITION_WAIT_MODE_RESET);
	CTDwCommandWrite(m_bsn, axis, CTD_ALARM_STOP_ENABLE_MODE_SET);
	// データ設定
	CTDwDataHalfWrite(m_bsn, axis, CTD_RANGE_WRITE, 8191);
	CTDwDataHalfWrite(m_bsn, axis, CTD_START_STOP_SPEED_DATA_WRITE, 819);
	CTDwDataHalfWrite(m_bsn, axis, CTD_OBJECT_SPEED_DATA_WRITE, 8191);
	CTDwDataHalfWrite(m_bsn, axis, CTD_RATE1_DATA_WRITE, 55);
	// アドレス設定
	CTDwDataFullWrite(m_bsn, axis, CTD_INTERNAL_COUNTER_WRITE, 0);
	CTDwDataFullWrite(m_bsn, axis, CTD_EXTERNAL_COUNTER_WRITE, 0);
}

/*-----------------------------------------------
*
* 速度を変更
* rate：加減速時間 [ms]
*
-----------------------------------------------*/
void Motor::change_speed(uint16_t axis, Speed speed)
{
	/* 現在の速度と変わらなかったら何も処理を行わないで終了 */
	if (speed == m_pre_speed)
		return;

	/* エラーチェック */
	if (speed.m_ss >= speed.m_object)
	{
		char str[100];
		snprintf(str, 100, "%s\n\ninvalid speed (ss %lf, object %lf)",
				 __FUNCSIG__, speed.m_ss, speed.m_object);
		print_err(str);
	}
	if (speed.m_rate <= 0)
	{
		char str[100];
		snprintf(str, 100, "%s\n\ninvalid rate (%lf)",
				 __FUNCSIG__, speed.m_rate);
		print_err(str);
	}

	/* 書き込むデータを計算 */
	uint16_t range_data = (8192E3 / speed.m_object > 8191) ? 8191 : 8192E3 / speed.m_object;
	double f_unit = 1000 / (double)range_data;
	uint16_t ss_data = speed.m_ss / f_unit;
	uint16_t object_data = speed.m_object / f_unit;
	uint16_t rate_data = speed.m_rate * (4.096E3) / (double)(object_data - ss_data);

	/* データを書き込む */
	CTDwDataHalfWrite(m_bsn, axis, CTD_RANGE_WRITE, range_data);
	CTDwDataHalfWrite(m_bsn, axis, CTD_START_STOP_SPEED_DATA_WRITE, ss_data);
	CTDwDataHalfWrite(m_bsn, axis, CTD_OBJECT_SPEED_DATA_WRITE, object_data);
	CTDwDataHalfWrite(m_bsn, axis, CTD_RATE1_DATA_WRITE, rate_data);
}

/*-----------------------------------------------
*
* ビジーステータスを取得
*
-----------------------------------------------*/
bool Motor::get_busy(uint16_t axis)
{
	uint8_t drive_status;
	CTDwGetDriveStatus(m_bsn, axis, &drive_status);
	return drive_status & 1;
}

/*-----------------------------------------------
*
* 内部カウンタ取得
*
-----------------------------------------------*/
int32_t Motor::get_int_cnt(uint16_t axis)
{
	int32_t int_cnt;
	CTDwDataFullRead(m_bsn, axis, CTD_INTERNAL_COUNTER_READ, (uint32_t *)&int_cnt);
	return int_cnt;
}

/*-----------------------------------------------
*
* 内部カウンタ書き込み
*
-----------------------------------------------*/
void Motor::set_int_cnt(uint16_t axis, int32_t int_cnt)
{
	CTDwDataFullWrite(m_bsn, axis, CTD_INTERNAL_COUNTER_WRITE, (uint32_t)int_cnt);
	printf("[axis %d] int_cnt was set to %5d\n", axis + 1, int_cnt);
}

/*-----------------------------------------------
*
* 指定数のパルスを出力する
*
-----------------------------------------------*/
void Motor::preset_pulse_drive(uint16_t axis, int64_t pulse)
{
	if (pulse > 0)
		CTDwDataFullWrite(m_bsn, axis, CTD_PLUS_PRESET_PULSE_DRIVE, abs(pulse));
	else if (pulse < 0)
		CTDwDataFullWrite(m_bsn, axis, CTD_MINUS_PRESET_PULSE_DRIVE, abs(pulse));
}

/*-----------------------------------------------
*
* リミットスイッチが押されるまで駆動する
*
-----------------------------------------------*/
void Motor::drive_limit_switch(uint16_t axis, DIRECTION direction)
{
	if (direction == PLUS)
		CTDwDataFullWrite(m_bsn, axis, CTD_PLUS_SIGNAL_SEARCH1_DRIVE, 0);
	else
		CTDwDataFullWrite(m_bsn, axis, CTD_MINUS_SIGNAL_SEARCH1_DRIVE, 0);
}

/*-----------------------------------------------
*
* 内部カウンタが target_cnt になるようにモータを駆動する
*
-----------------------------------------------*/
void Motor::drive_int_cnt(uint16_t axis, int32_t target_cnt)
{
	int32_t int_cnt = get_int_cnt(axis); /* 内部カウンタ取得 */
	int64_t diff = target_cnt - int_cnt; /* 偏差を計算 */
	preset_pulse_drive(axis, diff);		 /* パルスを出力 */
}

/*-----------------------------------------------
*
* 減速停止
*
-----------------------------------------------*/
void Motor::slow_down_stop(uint16_t axis)
{
	CTDwDataHalfWrite(m_bsn, axis, CTD_SLOW_DOWN_STOP, 0);
}