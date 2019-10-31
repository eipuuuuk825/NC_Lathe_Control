﻿#include <windows.h>
#include <tchar.h>
#include <conio.h> // kbhit
#include <stdio.h>
#include "CTDw.h"

const TCHAR szTitle[] = _T("Sample");

static void ResultMessage(LPCTSTR lpszbuf);
static void ErrorMessage(WORD wBsn);
static BOOL CTD_InitAxis(WORD wBsn, WORD wAxis);

int main(void)
{
	TCHAR szBuf[1024];
	BYTE bData[256];
	WORD wBsn, wAxis;
	BYTE b;
	DWORD dwCount;

	//---------------------------------------------------------------
	// CTD.Dll を開く
	//---------------------------------------------------------------
	if (FALSE == CTDwDllOpen())
	{
		wsprintf(szBuf, _T("DLL load error or driver open error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1; // DLLのロードまたはドライバのオープンに失敗しました
	}
	//---------------------------------------------------------------
	// デバイスの使用を宣言する
	//---------------------------------------------------------------
	wBsn = 0;
	if (-1 == CTDwCreate(wBsn))
	{
		wsprintf(szBuf, _T("Create error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;
	}
	wsprintf(szBuf,
			 _T("CTD Created\n")
			 _T("BSN Number %d"),
			 wBsn);
	ResultMessage(szBuf);
	//---------------------------------------------------------------
	// 第１軸を選択
	//---------------------------------------------------------------
	wAxis = CTD_AXIS_1;
	//---------------------------------------------------------------
	// 制御軸の初期化
	//---------------------------------------------------------------
	if (FALSE == CTD_InitAxis(wBsn, wAxis))
	{
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf(szBuf,
			 _T("CTD\n")
			 _T("制御軸 %d を初期化しました"),
			 wAxis);
	ResultMessage(szBuf);


	///* メカニカルシグナルを表示 */
	//while (!kbhit())
	//{
	//	if (FALSE == CTDwGetMechanicalSignal(wBsn, wAxis, &b))
	//	{
	//		ErrorMessage(wBsn);
	//		return 1;
	//	}
	//	printf("-SLM %d, -ELM %d\n", (b >> 3) & 1, (b >> 1) & 1);
	//}

	//---------------------------------------------------------------
	// リミットスイッチが押されるまで駆動
	//---------------------------------------------------------------
	if (FALSE == CTDwDataFullWrite(
					 wBsn, wAxis, CTD_PLUS_SIGNAL_SEARCH1_DRIVE, 300))
	{
		ErrorMessage(wBsn);
		return 1;
	}

	//---------------------------------------------------------------
	// デバイスを解放する
	//---------------------------------------------------------------
	if (FALSE == CTDwClose(wBsn))
	{
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf(szBuf,
			 _T("CTD Close\n")
			 _T("Bsn %d"),
			 wBsn);
	ResultMessage(szBuf);

	//---------------------------------------------------------------
	// CTD.Dll を閉じる
	//---------------------------------------------------------------
	if (FALSE == CTDwDllClose())
	{
		wsprintf(szBuf, _T("DLL Unload error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1; // DLLのアンロードに失敗しました
	}

	return 0;
}
//-------------------------------------------------------------------
// 制御軸の初期化
//-------------------------------------------------------------------
// 引  数		wBsn	ＢＳＮ（ボードセレクトナンバー）
//				wAxis	制御軸
//             	CTD_AXIS_1 ～ CTD_AXIS_4 を指定します
//-------------------------------------------------------------------
static BOOL CTD_InitAxis(WORD wBsn, WORD wAxis)
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
//-------------------------------------------------------------------
// 処理結果を表示
//-------------------------------------------------------------------
static void ResultMessage(LPCTSTR lpszbuf)
{
	MessageBox(NULL, lpszbuf, szTitle, MB_OK | MB_ICONINFORMATION);
}
//-------------------------------------------------------------------
// エラーメッセージを表示
//-------------------------------------------------------------------
static void ErrorMessage(WORD wBsn)
{
	DWORD dwRes = CTDwGetLastError(wBsn);
	TCHAR szbuf[50];

	wsprintf(szbuf, _T("CTD error\n\n")
					_T("Error code: %08lXh"),
			 dwRes);

	MessageBox(NULL, szbuf, szTitle, MB_OK | MB_ICONSTOP);
}
