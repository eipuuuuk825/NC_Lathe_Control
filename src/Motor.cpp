#include "../inc/Motor.hpp"
#include "../inc/Standard.hpp"

/*-----------------------------------------------
*
* 初期化処理
*
-----------------------------------------------*/
MOTOR::MOTOR(uint16_t Bsn, size_t MaxAxisNum)
{
	m_Bsn = Bsn;	   /* メンバの設定 */
	CTDwDllOpen();	 /* CTD.Dll を開く */
	CTDwCreate(m_Bsn); /* デバイスの使用を宣言する */

	for (size_t i = 0; i < MaxAxisNum; i++)
	{
		InitAxis(i);	 /* 制御軸の初期化 */
		SetIntCnt(i, 0); /* 内部カウンタ初期化 */
	}
}

/*-----------------------------------------------
*
* 終了処理
*
-----------------------------------------------*/
MOTOR::~MOTOR(void)
{
	CTDwClose(0);   /* デバイスを解放する */
	CTDwDllClose(); /* CTD.Dll を閉じる */
}

/*-----------------------------------------------
*
* 制御軸の初期化
*
-----------------------------------------------*/
void MOTOR::InitAxis(uint16_t Axis)
{
	// MODE1 SET
	CTDwMode1Write(m_Bsn, Axis, 0b01001001);
	// MODE2 SET
	CTDwMode2Write(m_Bsn, Axis, 0x3F);
	// モード設定
	CTDwCommandWrite(m_Bsn, Axis, CTD_INPOSITION_WAIT_MODE_RESET);
	CTDwCommandWrite(m_Bsn, Axis, CTD_ALARM_STOP_ENABLE_MODE_SET);
	// データ設定
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_RANGE_WRITE, 8191);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_START_STOP_SPEED_DATA_WRITE, 819);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_OBJECT_SPEED_DATA_WRITE, 8191);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_RATE1_DATA_WRITE, 55);
	// アドレス設定
	CTDwDataFullWrite(m_Bsn, Axis, CTD_INTERNAL_COUNTER_WRITE, 0);
	CTDwDataFullWrite(m_Bsn, Axis, CTD_EXTERNAL_COUNTER_WRITE, 0);
}

/*-----------------------------------------------
*
* 速度を変更
* rate：加減速時間 [ms]
*
-----------------------------------------------*/
void MOTOR::ChangeSpeed(uint16_t Axis, SPEED Speed)
{
	/* エラーチェック */
	if (Speed.m_SS >= Speed.m_Object)
	{
		char str[100];
		snprintf(str, 100, "%s\n\ninvalid speed (SS %lf, Object %lf)",
				 __FUNCSIG__, Speed.m_SS, Speed.m_Object);
		PrintError(str);
		exit(EXIT_FAILURE);
	}
	if (Speed.m_Rate <= 0)
	{
		char str[100];
		snprintf(str, 100, "%s\n\ninvalid Rate (%lf)",
				 __FUNCSIG__, Speed.m_Rate);
		PrintError(str);
		exit(EXIT_FAILURE);
	}

	/* 書き込むデータを計算 */
	uint16_t RangeData = (8192E3 / Speed.m_Object > 8191) ? 8191 : 8192E3 / Speed.m_Object;
	double Funit = 1000 / (double)RangeData;
	uint16_t SS_Data = Speed.m_SS / Funit;
	uint16_t ObjectData = Speed.m_Object / Funit;
	uint16_t RateData = Speed.m_Rate * (4.096E3) / (double)(ObjectData - SS_Data);

	/* データを書き込む */
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_RANGE_WRITE, RangeData);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_START_STOP_SPEED_DATA_WRITE, SS_Data);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_OBJECT_SPEED_DATA_WRITE, ObjectData);
	CTDwDataHalfWrite(m_Bsn, Axis, CTD_RATE1_DATA_WRITE, RateData);
}

/*-----------------------------------------------
*
* ビジーステータスを取得
*
-----------------------------------------------*/
bool MOTOR::GetBusy(uint16_t Axis)
{
	uint8_t driveStatus;
	CTDwGetDriveStatus(m_Bsn, Axis, &driveStatus);
	return driveStatus & 1;
}

/*-----------------------------------------------
*
* 内部カウンタ取得
*
-----------------------------------------------*/
int32_t MOTOR::GetIntCnt(uint16_t Axis)
{
	int32_t IntCnt;
	CTDwDataFullRead(m_Bsn, Axis, CTD_INTERNAL_COUNTER_READ, (uint32_t *)&IntCnt);
	return IntCnt;
}

/*-----------------------------------------------
*
* 内部カウンタ書き込み
*
-----------------------------------------------*/
void MOTOR::SetIntCnt(uint16_t Axis, int32_t IntCnt)
{
	CTDwDataFullWrite(m_Bsn, Axis, CTD_INTERNAL_COUNTER_WRITE, (uint32_t)IntCnt);
	printf("[Axis %d] IntCnt was set to %5d\n", Axis + 1, IntCnt);
}

/*-----------------------------------------------
*
* 指定数のパルスを出力する
*
-----------------------------------------------*/
void MOTOR::PresetPulseDrive(uint16_t Axis, int64_t Pulse)
{
	if (Pulse > 0)
		CTDwDataFullWrite(m_Bsn, Axis, CTD_PLUS_PRESET_PULSE_DRIVE, abs(Pulse));
	else if (Pulse < 0)
		CTDwDataFullWrite(m_Bsn, Axis, CTD_MINUS_PRESET_PULSE_DRIVE, abs(Pulse));
}

/*-----------------------------------------------
*
* リミットスイッチが押されるまで駆動する
*
-----------------------------------------------*/
void MOTOR::DriveLimitSwitch(uint16_t Axis, DIRECTION Direction)
{
	if (Direction == PLUS)
		CTDwDataFullWrite(m_Bsn, Axis, CTD_PLUS_SIGNAL_SEARCH1_DRIVE, 0);
	else
		CTDwDataFullWrite(m_Bsn, Axis, CTD_MINUS_SIGNAL_SEARCH1_DRIVE, 0);
}

/*-----------------------------------------------
*
* 内部カウンタが TargetCnt になるようにモータを駆動する
*
-----------------------------------------------*/
void MOTOR::DriveIntCnt(uint16_t Axis, int32_t TargetCnt)
{
	int32_t IntCnt = GetIntCnt(Axis);  /* 内部カウンタ取得 */
	int64_t Diff = TargetCnt - IntCnt; /* 偏差を計算 */
	PresetPulseDrive(Axis, Diff);	  /* パルスを出力 */
}