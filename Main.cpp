#include <windows.h>
#include <tchar.h>
#include <conio.h> // kbhit
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "CTDw.h"

/*-----------------------------------------------
*
* プロトタイプ宣言
*
-----------------------------------------------*/
void Initialize(void);
BOOL CTD_InitAxis(WORD wBsn, WORD wAxis);
void Terminate(void);
BOOL ChangeSpeed(WORD wBsn, WORD wAxis, DOUBLE ss, DOUBLE object, DOUBLE rate);
BOOL GetBusy(WORD wBsn, WORD wAxis);
unsigned int GetIntCnt(WORD wBsn, WORD wAxis);
void Drive(WORD wBsn, WORD wAxis, short TargetCnt);
void PresetPulseDrive(WORD wBsn, WORD wAxis, short Pulse);
void PrintError(const char *msg);
std::vector<short> LoadTargetData(const char *path);

int main(void)
{
	/* データを設定 */
	std::vector<short> TargetData = LoadTargetData("");

	/* 初期化 */
	Initialize();

	// リミットスイッチが押されるまで駆動
	// CTDwDataFullWrite(0, CTD_AXIS_3, CTD_PLUS_SIGNAL_SEARCH1_DRIVE, 0);
	// CTDwDataFullWrite(0, CTD_AXIS_4, CTD_PLUS_SIGNAL_SEARCH1_DRIVE, 0);

	ChangeSpeed(0, CTD_AXIS_3, 300, 301, 1);

	while (!TargetData.empty())
	{
		/* パルス出力 */
		if (!GetBusy(0, CTD_AXIS_3))
		{
			printf("Target %+5d\t", TargetData[0]);
			Drive(0, CTD_AXIS_3, TargetData[0]);
			TargetData.erase(TargetData.begin());
		}
		/* キー入力 */
		if (GetAsyncKeyState('Q'))
			break;
	}

	/* 終了処理 */
	Terminate();
	return 0;
}

//-------------------------------------------------------------------
// 制御軸の初期化
//-------------------------------------------------------------------
// 引  数		wBsn	ＢＳＮ（ボードセレクトナンバー）
//				wAxis	制御軸
//             	CTD_AXIS_1 ～ CTD_AXIS_4 を指定します
//-------------------------------------------------------------------
BOOL CTD_InitAxis(WORD wBsn, WORD wAxis)
{
	//---------------------------------------------------------------
	// MODE1 SET
	//
	// 減速開始ポイント検出方式				自動
	// パルス出力方式						２パルス方式
	// DIR   出力端子 						CWパルス  アクティブ Hi
	// PULSE 出力端子						CCWパルス アクティブ Hi
	// 検出対象信号							-LM-n（-ELM-n）ポジティブエッジ
	//---------------------------------------------------------------
	if (FALSE == CTDwMode1Write(wBsn, wAxis, 0b01001001))
		return FALSE;
	//---------------------------------------------------------------
	// MODE2 SET
	//
	// EXTERNAL COUNTER 入力仕様			None
	// DEND 入力信号アクティブレベル		High
	// DERR 入力信号アクティブレベル		High
	// -SLM 入力信号アクティブレベル		High
	// +SLM 入力信号アクティブレベル		High
	// -ELM 入力信号アクティブレベル		High
	// +ELM 入力信号アクティブレベル		High
	//---------------------------------------------------------------
	if (FALSE == CTDwMode2Write(wBsn, wAxis, 0x3F))
		return FALSE;
	//---------------------------------------------------------------
	// モード設定
	//
	// INPOSITION WAIT MODE RESET
	// ALARM STOP ENABLE MODE SET
	//---------------------------------------------------------------
	if (FALSE == CTDwCommandWrite(
					 wBsn, wAxis, CTD_INPOSITION_WAIT_MODE_RESET))
		return FALSE;
	if (FALSE == CTDwCommandWrite(
					 wBsn, wAxis, CTD_ALARM_STOP_ENABLE_MODE_SET))
		return FALSE;
	//---------------------------------------------------------------
	// データ設定
	//
	// RANGE DATA				100		出力周波数設定単位 	1000÷100=10PPS
	// START/STOP SPEED DATA	100		開始停止周波数 		100×10PPS=1000PPS
	// OBJECT SPEED DATA		4000	目的周波数 			4000×10PPS=40000PPS
	// RATE-1 DATA              2048	加速時間設定単位 	2048÷(4.096×10^6)=0.5mSec
	//									加速時間 			(4000-100)×0.5mSec = 1.95Sec
	// RATE-2 DATA        				デフォルト値 8191(1FFFh)
	//
	// この設定により RATE-1 DATA による直線加減速となります
	//---------------------------------------------------------------
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_RANGE_WRITE, 8191))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, 819))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, 8191))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_RATE1_DATA_WRITE, 55))
		return FALSE;
	//---------------------------------------------------------------
	// アドレス設定
	//
	// INTERNAL COUNTER 及び EXTERNAL COUNTER に 0h を書き込みます
	//---------------------------------------------------------------
	if (FALSE == CTDwDataFullWrite(
					 wBsn, wAxis, CTD_INTERNAL_COUNTER_WRITE, 0))
		return FALSE;
	if (FALSE == CTDwDataFullWrite(
					 wBsn, wAxis, CTD_EXTERNAL_COUNTER_WRITE, 0))
		return FALSE;

	return TRUE;
}

/*-----------------------------------------------
*
* 速度を変更
* rate：加減速時間 [ms]
*
-----------------------------------------------*/
BOOL ChangeSpeed(WORD wBsn, WORD wAxis, DOUBLE ss, DOUBLE object, DOUBLE rate)
{
	/* エラーチェック */
	if (ss >= object)
	{
		char str[100];
		snprintf(str, 100,
				 "ChangeSpeed\n"
				 "invalid speed (ss %lf, object %lf)",
				 ss, object);
		PrintError(str);
		exit(EXIT_FAILURE);
	}
	if (rate <= 0)
	{
		char str[100];
		snprintf(str, 100,
				 "ChangeSpeed\n"
				 "invalid rate (%lf)",
				 rate);
		PrintError(str);
		exit(EXIT_FAILURE);
	}

	/* 書き込むデータを計算 */
	WORD range_data = (8192E3 / object > 8191) ? 8191 : 8192E3 / object;
	DOUBLE Funit = 1000 / (DOUBLE)range_data;
	WORD ss_data = ss / Funit;
	WORD object_data = object / Funit;
	WORD rate_data = rate * (4.096E3) / (DOUBLE)(object_data - ss_data);

	/* データを書き込む */
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_RANGE_WRITE, range_data))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, ss_data))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, object_data))
		return FALSE;
	if (FALSE == CTDwDataHalfWrite(
					 wBsn, wAxis, CTD_RATE1_DATA_WRITE, rate_data))
		return FALSE;

	return TRUE;
}

/*-----------------------------------------------
*
* ビジーステータスを取得
*
-----------------------------------------------*/
BOOL GetBusy(WORD wBsn, WORD wAxis)
{
	BYTE driveStatus;
	CTDwGetDriveStatus(wBsn, wAxis, &driveStatus);
	return driveStatus & 1;
}

/*-----------------------------------------------
*
* 内部カウンタ取得
*
-----------------------------------------------*/
unsigned int GetIntCnt(WORD wBsn, WORD wAxis)
{
	DWORD IntCnt;
	CTDwDataFullRead(wBsn, wAxis, CTD_INTERNAL_COUNTER_READ, &IntCnt);
	printf("IntCnt %5d\t", IntCnt);
	return IntCnt;
}

/*-----------------------------------------------
*
* 初期化
*
-----------------------------------------------*/
void Initialize(void)
{
	/* キー入力があるまで停止 */
	printf("press any key.\n");
	while (!_kbhit())
		;

	/* CTD.Dll を開く */
	CTDwDllOpen();

	/* デバイスの使用を宣言する */
	CTDwCreate(0);

	/* 制御軸の初期化 */
	CTD_InitAxis(0, CTD_AXIS_3);
	// CTD_InitAxis(0, CTD_AXIS_4);

	/* 速度を変更 */
	ChangeSpeed(0, CTD_AXIS_3, 100, 1000, 1000);
	// ChangeSpeed(0, CTD_AXIS_4, 100, 1000, 1000);
}

/*-----------------------------------------------
*
* 終了処理
*
-----------------------------------------------*/
void Terminate(void)
{
	CTDwClose(0);   // デバイスを解放する
	CTDwDllClose(); // CTD.Dll を閉じる
}

/*-----------------------------------------------
*
* 内部カウンタが TargetCnt になるようにモータを駆動する
*
-----------------------------------------------*/
void Drive(WORD wBsn, WORD wAxis, short TargetCnt)
{
	/* 内部カウンタ取得 */
	unsigned int IntCnt = GetIntCnt(wBsn, wAxis);

	/* 偏差を計算 */
	short Diff = TargetCnt - (short)IntCnt;
	printf("Diff %+5d\n", Diff);

	/* パルスを出力 */
	PresetPulseDrive(wBsn, wAxis, Diff);
}

/*-----------------------------------------------
*
* 指定数のパルスを出力する
*
-----------------------------------------------*/
void PresetPulseDrive(WORD wBsn, WORD wAxis, short Pulse)
{
	/* パルスを出力 */
	if (Pulse > 0)
		CTDwDataFullWrite(wBsn, wAxis, CTD_PLUS_PRESET_PULSE_DRIVE, abs(Pulse));
	else if (Pulse < 0)
		CTDwDataFullWrite(wBsn, wAxis, CTD_MINUS_PRESET_PULSE_DRIVE, abs(Pulse));
}

/*-----------------------------------------------
*
* エラーメッセージ
*
-----------------------------------------------*/
void PrintError(const char *msg)
{
	MessageBox(NULL, _T(msg), _T("Error"), MB_OK | MB_ICONSTOP);
}

/*-----------------------------------------------
*
* 目標値データを読み込む
*
-----------------------------------------------*/
std::vector<short> LoadTargetData(const char *path)
{
	std::vector<short> TargetData;
	for (size_t i = 0; i < 1000 / 10; i++)
		TargetData.emplace_back((i + 1) * 10);
	for (size_t i = 0; i < TargetData.size(); i++)
		printf("%d\n", TargetData[i]);
	return TargetData;
}