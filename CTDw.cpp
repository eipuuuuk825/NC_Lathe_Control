//===================================================================
// CTDw.c
//-------------------------------------------------------------------
// CTD制御
// Copyright CocmoTechs Co.,Ltd.
//
// Date 09.16.2015
//===================================================================
#pragma comment(lib,"winmm.lib")
#pragma warning(disable:4996)

#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>

#include "CTDw.h"

HINSTANCE hDLL;
unsigned short BackIO;

CTD_SPD_PARAM	SpdParam[0x110];

char buffx[500];

// -- スタティック関数 & スタティック変数（定数）
static BOOL CTDwGetDllExportFunc( VOID );

//// -- エクスポート関数アドレスの格納用変数
BOOL	(WINAPI *pCTDGetLibVersion)(PBYTE);
BOOL	(WINAPI *pCTDGetDrvVersion)(PBYTE);
int 	(WINAPI *pCTDCreate)(WORD);
BOOL	(WINAPI *pCTDClose)(WORD);
BOOL	(WINAPI *pCTDGetRomVersion)(WORD,PBYTE);
BOOL	(WINAPI *pCTDInPort)(WORD,WORD,WORD,PBYTE);
BOOL	(WINAPI *pCTDOutPort)(WORD,WORD,WORD,BYTE);
BOOL	(WINAPI *pCTDGetDriveStatus)(WORD,WORD,PBYTE);
BOOL	(WINAPI *pCTDGetEndStatus)(WORD,WORD,PBYTE);
BOOL	(WINAPI *pCTDGetMechanicalSignal)(WORD,WORD,PBYTE);
BOOL	(WINAPI *pCTDGetUniversalSignal)(WORD,WORD,PBYTE);
BOOL	(WINAPI *pCTDMode1Write)(WORD,WORD,BYTE);
BOOL	(WINAPI *pCTDMode2Write)(WORD,WORD,BYTE);
BOOL	(WINAPI *pCTDUniversalSignalWrite)(WORD,WORD,BYTE);
BOOL	(WINAPI *pCTDDataHalfRead)(WORD,WORD,BYTE,PWORD);
BOOL	(WINAPI *pCTDDataFullRead)(WORD,WORD,BYTE,PDWORD);
BOOL	(WINAPI *pCTDCommandWrite)(WORD,WORD,BYTE);
BOOL	(WINAPI *pCTDDataHalfWrite)(WORD,WORD,BYTE,WORD);
BOOL	(WINAPI *pCTDDataFullWrite)(WORD,WORD,BYTE,DWORD);
int     (WINAPI *pCTDCallBack)(HANDLE,void (WINAPI *)(void));
int     (WINAPI *pCTDFreeCallBack)(void);
DWORD	(WINAPI *pCTDGetLastError)(WORD);
BOOL	(WINAPI *pCTDLineFallOut)(WORD,WORD,WORD);
BOOL	(WINAPI *pCTDGetIoRead)(WORD,PDWORD);
BOOL	(WINAPI *pCTDSetIoWrite)(WORD,WORD,PDWORD);
BOOL 	(WINAPI *pCTDPDataWrite)(WORD,WORD,BYTE,DWORD,DWORD);
BOOL 	(WINAPI *pCTDPDataRead)(WORD,WORD,BYTE,DWORD,DWORD);
BOOL 	(WINAPI *pCTDPDriveRead)(WORD,WORD,BYTE,DWORD);
BOOL 	(WINAPI *pCTDPOutPort)(WORD,WORD,BYTE,BYTE,DWORD);
BOOL 	(WINAPI *pCTDPInPort)(WORD,WORD,BYTE,BYTE,BYTE,BYTE,DWORD,DWORD);
BOOL 	(WINAPI *pCTDPGStartStop)(WORD,BYTE);
BOOL 	(WINAPI *pCTDPGAllStep)(WORD,DWORD);
BOOL 	(WINAPI *pCTDPGWait)(WORD,DWORD,DWORD);
BOOL 	(WINAPI *pCTDPGEnd)(WORD,DWORD);
BOOL 	(WINAPI *pCTDPGStatus)(WORD,PDWORD,PBYTE);
BOOL 	(WINAPI *pCTDStartSignalWrite)(WORD,WORD);
BOOL 	(WINAPI *pCTDGetAxisStatus)(WORD, PBYTE);
BOOL 	(WINAPI *pCTDGetAxisAllPort)(WORD, WORD, PDWORD);
BOOL 	(WINAPI *pCTDDeviceStatus)(WORD,PBYTE);

int		(WINAPI *pCTDSetControl)(WORD, WORD, BYTE);
int		(WINAPI *pCTDGetControl)(WORD, WORD, PBYTE);

int		(WINAPI *pCTDSetSoftLimit)(WORD, WORD, DWORD, DWORD, WORD, WORD);
int		(WINAPI *pCTDGetSoftLimit)(WORD, WORD, PDWORD, PDWORD, PWORD, PWORD);

int		(WINAPI *pCTDGetVersion)(PBYTE, PBYTE);
int		(WINAPI *pCTDGetBoardStatus)(WORD, WORD, PBYTE);
int		(WINAPI *pCTDGetCounter)(WORD, WORD, BYTE, PDWORD);

int		(WINAPI *pCTDSetSpeedParameter)(WORD, WORD, PCTDSPDPARAMETER);
int		(WINAPI *pCTDGetSpeedParameter)(WORD, WORD, PCTDSPDPARAMETER);
int		(WINAPI *pCTDSpeedOverride)(WORD, WORD, DWORD);
int		(WINAPI *pCTDGetNowSpeed)(WORD, WORD, PDWORD);

int		(WINAPI *pCTDDriveStart)(WORD, WORD, BYTE, DWORD);
int		(WINAPI *pCTDDriveStop)(WORD, WORD, BYTE);
int		(WINAPI *pCTDPulseOverride)(WORD, WORD, DWORD);
int		(WINAPI *pCTDSyncONOFF)(WORD, BYTE);

int		(WINAPI *pCTDORGSignalStart)(WORD, WORD, BYTE);
int		(WINAPI *pCTDORGSignalStop)(WORD, WORD, BYTE);
int		(WINAPI *pCTDORGGetStatus)(WORD, WORD, PBYTE);
int		(WINAPI *pCTDORGSetPosiData)(WORD, WORD, DWORD);

int		(WINAPI *pCTDSetDriveONOFF)(WORD, WORD, BYTE);
int		(WINAPI *pCTDSetDrstONOFF)(WORD, WORD, BYTE);

int		(WINAPI *pCTDGetALMCode)(WORD, WORD, WORD, PBYTE);
int		(WINAPI *pCTDALMCLR)(WORD, WORD, WORD);
int		(WINAPI *pCTDParaCont)(WORD, WORD, BYTE, WORD, WORD, DWORD, PDWORD);
int		(WINAPI *pCTDEvent)(WORD, WORD, BYTE, WORD, WORD, PDWORD);

//14.12.17
void	(WINAPI *pCTLock)(void);
void	(WINAPI *pCTUnLock)(void);
int		(WINAPI *pCTDGetStatusCounterALL)(WORD, PBYTE, PBYTE, PBYTE, PDWORD, PDWORD);

//===================================================================
static BOOL DebugPrint(LPSTR pString)
{
	OutputDebugStringA((LPCSTR)pString);
	return TRUE;
}
//===================================================================
// 説  明		DLL をアンロード
// 引  数		なし
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwDllClose( VOID )
{

#if _DEBUG 
	DebugPrint("DllClose \n");
#endif

	if ( hDLL == NULL ) return FALSE;
	if ( FreeLibrary(hDLL) == TRUE ) {
		hDLL = NULL;
		return TRUE;
	}
	return FALSE;
}
//===================================================================
// 説  明		DLL をロード
// 引  数		なし
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY  CTDwDllOpen( VOID )
{
	if ( hDLL != NULL ) return FALSE;
	hDLL = NULL;
	hDLL = (HMODULE)LoadLibrary(CTD_DLL_NAME);
	if ( hDLL == NULL ) return FALSE;
	if ( CTDwGetDllExportFunc() == FALSE ) {
		CTDwDllClose();
		return FALSE;
	}

#if _DEBUG 
	DebugPrint("DllOpen \n");
#endif

	return TRUE;
}
//===================================================================
// 説  明		バージョン番号取得
// 引  数		pbLibVer	DLL バージョンを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwGetLibVersion( PBYTE pbLibVer)
{
BOOL bln;

	if ( hDLL == NULL ) return FALSE;
	bln = (*pCTDGetLibVersion)(pbLibVer);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetLibVersion pbLibVer= %s \n", pbLibVer);
#else
	sprintf_s((char*)buffx, 500, "GetLibVersion pbLibVer= %s \n", pbLibVer);
#endif

	DebugPrint(buffx);
#endif

	return bln;
}
//===================================================================
// 説  明		バージョン番号取得
// 引  数		pbDrvVer	DRV バージョンを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwGetDrvVersion( PBYTE pbDrvVer)
{
BOOL bln;

	if ( hDLL == NULL ) return FALSE;
	bln = (*pCTDGetDrvVersion)(pbDrvVer);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetDrvVersion pbDrvVer= %s \n", pbDrvVer);
#else
	sprintf_s((char*)buffx, 500, "GetDrvVersion pbDrvVer= %s \n", pbDrvVer);
#endif

	DebugPrint(buffx);
#endif


	return bln;
}
//===================================================================
// 説  明		デバイスの使用を宣言
// 引  数		wBsn	ＢＳＮ（ボードセレクトナンバー）
//						
// 戻り値		TRUE	成功
//				-1		失敗
//===================================================================
BOOL APIENTRY CTDwCreate( WORD wBsn )
{
unsigned short wb;
unsigned char i;

	if ( hDLL == NULL ) return FALSE;
	if ((*pCTDCreate)(wBsn) == -1){	//-1=FALSE  Else = TRUE
		return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "Create wBsn= %d \n", wBsn);
#else
	sprintf_s((char*)buffx, 500, "Create wBsn= %d \n", wBsn);
#endif

	DebugPrint(buffx);
#endif

	//USPG28 or USPG48
	if (wBsn >= 0x100)
		wb = 2;
	else
		wb = 4;

	for(i=0;i<wb;i++)
		SpdParam[wBsn].dObjSpd[i] = 100.0;

	return TRUE;
}
//===================================================================
// 説  明		デバイスの解放
// 引  数		wBsn	ボードセレクトナンバー
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwClose(WORD wBsn)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	bln = (*pCTDClose)(wBsn);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "Close wBsn= %d \n", wBsn);
#else
	sprintf_s((char*)buffx, 500, "Close wBsn= %d \n", wBsn);
#endif

	DebugPrint(buffx);
#endif

	return bln;
}
//===================================================================
// 説  明		バージョン番号取得
// 引  数		pbRomVer	ROM バージョンを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwGetRomVersion(WORD wBsn, PBYTE pbRomVer)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetRomVersion)(wBsn,pbRomVer);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "pbRomVer wBsn=%d pbRomVer= %s \n", wBsn, pbRomVer);
#else
	sprintf_s((char*)buffx, 500, "pbRomVer wBsn=%d pbRomVer= %s \n", wBsn, pbRomVer);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		デバイスの状態を取得
// 引  数		wBsn			ボードセレクトナンバー
//				pdStatus		デバイスの状態を格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwDeviceStatus( WORD wBsn, PBYTE pbStatus)
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	return (*pCTDDeviceStatus)(wBsn,pbStatus);
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
}
//===================================================================
// 説  明		エラーコード取得
// 引  数		wBsn	ボードセレクトナンバー
// 戻り値		エラーコード
//===================================================================
DWORD APIENTRY CTDwGetLastError(WORD wBsn)
{
DWORD iError;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	iError = (*pCTDGetLastError)(wBsn);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetLastError wBsn= %d iError=%d \n", wBsn,iError);
#else
	sprintf_s((char*)buffx, 500, "GetLastError wBsn= %d iError=%d \n", wBsn,iError);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return iError;
}
//===================================================================
// 説  明		コネクタ抜けた時の出力動作設定
// 引  数		wBsn	BSN
//				wData	動作パラメータ
//						 ０：減速停止命令出力
//						 １：非常停止命令出力
//						 ２：何も出力しない
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwLineFallOut(WORD wBsn, WORD wAxis, WORD wData)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDLineFallOut)(wBsn,wAxis,wData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "LineFallOut wBsn= %d wAxis= %d wData= %d \n",wBsn, wAxis, wData);
#else
	sprintf_s((char*)buffx, 500, "LineFallOut wBsn= %d wAxis= %d wData= %d \n",wBsn, wAxis, wData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		読み込み実行
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				wPort	対象ポート番号
//				pbData	読み込んだデータを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwInPort( WORD wBsn, WORD wAxis ,WORD wPort, PBYTE pbData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDInPort)(wBsn,wAxis,wPort,pbData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "InPort wBsn= %d wAxis= %d wPort= %d pbData= 0x%x \n",wBsn, wAxis ,wPort, *pbData);
#else
	sprintf_s((char*)buffx, 500, "InPort wBsn= %d wAxis= %d wPort= %d pbData= 0x%x \n",wBsn, wAxis ,wPort, *pbData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		書き込み実行
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				wPort	対象ポート番号
//				bData	書き込む値
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwOutPort( WORD wBsn, WORD wAxis ,WORD wPort, BYTE bData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDOutPort)(wBsn,wAxis,wPort,bData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "OutPort wBsn= %d wAxis= %d wPort= %d bData= 0x%x \n",wBsn, wAxis ,wPort, bData);
#else
	sprintf_s((char*)buffx, 500, "OutPort wBsn= %d wAxis= %d wPort= %d bData= 0x%x \n",wBsn, wAxis ,wPort, bData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		ドライブステータス取得
// 引  数		wBsn		ボードセレクトナンバー
//				wAxis		対象制御軸
//				pbStatus	情報を格納する領域へのポインタ	
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetDriveStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetDriveStatus)(wBsn,wAxis,pbStatus);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetDriveStatus wBsn= %d wAxis= %d pbStatus= 0x%x \n",wBsn, wAxis, *pbStatus);
#else
	sprintf_s((char*)buffx, 500, "GetDriveStatus wBsn= %d wAxis= %d pbStatus= 0x%x \n",wBsn, wAxis, *pbStatus);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		エンドステータス取得
// 引  数		wBsn		ボードセレクトナンバー
//				wAxis		対象制御軸
//				pbStatus	情報を格納する領域へのポインタ	
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetEndStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetEndStatus)(wBsn,wAxis,pbStatus);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetEndStatus wBsn= %d wAxis= %d pbStatus= 0x%x \n",wBsn, wAxis, *pbStatus);
#else
	sprintf_s((char*)buffx, 500, "GetEndStatus wBsn= %d wAxis= %d pbStatus= 0x%x \n",wBsn, wAxis, *pbStatus);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		メカニカルシグナル取得
// 引  数		wBsn		ボードセレクトナンバー
//				wAxis		対象制御軸
//				pbSignal	情報を格納する領域へのポインタ	
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetMechanicalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetMechanicalSignal)(wBsn,wAxis,pbSignal);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetMechanicalSignal wBsn= %d wAxis= %d pbSignal= 0x%x \n",wBsn, wAxis, *pbSignal);
#else
	sprintf_s((char*)buffx, 500, "GetMechanicalSignal wBsn= %d wAxis= %d pbSignal= 0x%x \n",wBsn, wAxis, *pbSignal);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		ユニバーサルシグナル取得
// 引  数		wBsn		ボードセレクトナンバー
//				wAxis		対象制御軸
//				pbSignal	情報を格納する領域へのポインタ	
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetUniversalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetUniversalSignal)(wBsn,wAxis,pbSignal);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetUniversalSignal wBsn= %d wAxis= %d pbSignal= 0x%x \n",wBsn, wAxis, *pbSignal);
#else
	sprintf_s((char*)buffx, 500, "GetUniversalSignal wBsn= %d wAxis= %d pbSignal= 0x%x \n",wBsn, wAxis, *pbSignal);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		モード１書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bMode	書き込むデータ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwMode1Write( WORD wBsn, WORD wAxis, BYTE bMode )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDMode1Write)(wBsn,wAxis,bMode);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "Mode1Write wBsn= %d wAxis= %d bMode= 0x%x \n",wBsn, wAxis, bMode);
#else
	sprintf_s((char*)buffx, 500, "Mode1Write wBsn= %d wAxis= %d bMode= 0x%x \n",wBsn, wAxis, bMode);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		モード２書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bMode	書き込むデータ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwMode2Write( WORD wBsn, WORD wAxis, BYTE bMode )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDMode2Write)(wBsn,wAxis,bMode);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "Mode2Write wBsn= %d wAxis= %d bMode= 0x%x \n",wBsn, wAxis, bMode);
#else
	sprintf_s((char*)buffx, 500, "Mode2Write wBsn= %d wAxis= %d bMode= 0x%x \n",wBsn, wAxis, bMode);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		ユニバーサルシグナル書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bSignal	書き込むデータ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL  APIENTRY CTDwUniversalSignalWrite( WORD wBsn, WORD wAxis, BYTE bSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDUniversalSignalWrite)(wBsn,wAxis,bSignal);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "UniversalSignalWrite wBsn= %d wAxis= %d bSignal= 0x%x \n",wBsn, wAxis, bSignal);
#else
	sprintf_s((char*)buffx, 500, "UniversalSignalWrite wBsn= %d wAxis= %d bSignal= 0x%x \n",wBsn, wAxis, bSignal);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		２バイトデータ読み込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bCmd	読み込み時に実行するコマンド
//				pwData	読み込んだデータを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY  CTDwDataHalfRead( WORD wBsn, WORD wAxis, BYTE bCmd, PWORD pwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDDataHalfRead)(wBsn,wAxis,bCmd,pwData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataHalfRead wBsn= %d wAxis=%d bCmd= 0x%x pwData= %d \n",wBsn, wAxis, bCmd, *pwData);
#else
	sprintf_s((char*)buffx, 500, "DataHalfRead wBsn= %d wAxis=%d bCmd= 0x%x pwData= %d \n",wBsn, wAxis, bCmd, *pwData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
BOOL  APIENTRY CTDwDataRead( WORD wBsn, WORD wAxis, BYTE bCmd, PBYTE pbData )
{
BOOL bln;
WORD wData;

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = CTDwDataHalfRead(wBsn,wAxis,bCmd,&wData);

    *pbData = (BYTE)wData;

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataRead wBsn= %d wAxis=%d bCmd= 0x%x pbData= %d \n",wBsn, wAxis, bCmd, *pbData);
#else
	sprintf_s((char*)buffx, 500, "DataRead wBsn= %d wAxis=%d bCmd= 0x%x pbData= %d \n",wBsn, wAxis, bCmd, *pbData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		４バイトデータ読み込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bCmd	読み込み時に実行するコマンド
//				pdwData	読み込んだデータを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwDataFullRead( WORD wBsn, WORD wAxis, BYTE bCmd, PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDDataFullRead)(wBsn,wAxis,bCmd,pdwData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataFullRead wBsn= %d wAxis= %d bCmd= 0x%x pdwData= %d \n",wBsn, wAxis, bCmd, *pdwData);
#else
	sprintf_s((char*)buffx, 500, "DataFullRead wBsn= %d wAxis= %d bCmd= 0x%x pdwData= %d \n",wBsn, wAxis, bCmd, *pdwData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		コマンド書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bCmd	コマンド
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwCommandWrite( WORD wBsn, WORD wAxis, BYTE bCmd )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDCommandWrite)(wBsn,wAxis,bCmd);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "CommandWrite wBsn= %d wAxis= %d bCmd= 0x%x \n",wBsn, wAxis, bCmd);
#else
	sprintf_s((char*)buffx, 500, "CommandWrite wBsn= %d wAxis= %d bCmd= 0x%x \n",wBsn, wAxis, bCmd);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		２バイトデータ書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bCmd	書き込み時に実行するコマンド
//				wData	書き込むデータ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwDataHalfWrite( WORD wBsn, WORD wAxis, BYTE bCmd, WORD wData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDDataHalfWrite)(wBsn,wAxis,bCmd,wData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataHalfWrite wBsn= %d wAxis= %d bCmd= 0x%x wData= %d \n",wBsn, wAxis, bCmd, wData);
#else
	sprintf_s((char*)buffx, 500, "DataHalfWrite wBsn= %d wAxis= %d bCmd= 0x%x wData= %d \n",wBsn, wAxis, bCmd, wData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
BOOL APIENTRY CTDwDataWrite( WORD wBsn, WORD wAxis, BYTE bCmd, BYTE bData )
{
BOOL bln;
WORD wData;

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    wData = bData;

	bln = CTDwDataHalfWrite(wBsn,wAxis,bCmd,wData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataWrite wBsn= %d wAxis= %d bCmd= 0x%x bData= %d \n",wBsn, wAxis, bCmd, bData);
#else
	sprintf_s((char*)buffx, 500, "DataWrite wBsn= %d wAxis= %d bCmd= 0x%x bData= %d \n",wBsn, wAxis, bCmd, bData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		４バイトデータ書き込み
// 引  数		wBsn	ボードセレクトナンバー
//				wAxis	対象制御軸
//				bCmd	書き込み時に実行するコマンド
//				dwData	書き込むデータ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwDataFullWrite( WORD wBsn, WORD wAxis, BYTE bCmd, DWORD dwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDDataFullWrite)(wBsn,wAxis,bCmd,dwData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DataFullWrite wBsn= %d wAxis= %d bCmd= 0x%x dwData= %d \n",wBsn, wAxis, bCmd, dwData);
#else
	sprintf_s((char*)buffx, 500, "DataFullWrite wBsn= %d wAxis= %d bCmd= 0x%x dwData= %d \n",wBsn, wAxis, bCmd, dwData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
// 説  明		IO読み出し
// 引  数		wBsn		BSN
//			    pdwData     32bit Data Read
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetIoRead( WORD wBsn,PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDGetIoRead)(wBsn,pdwData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetIoRead wBsn= %d pdwData= 0x%x \n",wBsn, *pdwData);
#else
	sprintf_s((char*)buffx, 500, "GetIoRead wBsn= %d pdwData= 0x%x \n",wBsn, *pdwData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
//badr =0:0byte 1:1byte 2:2byte 3:3byte

BOOL APIENTRY CTDwGetIoReadByte( WORD wBsn,BYTE badr,PBYTE pbData )
{
DWORD dwData;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if (CTDwGetIoRead(wBsn,&dwData) == FALSE){
 		return FALSE;
	}

	switch(badr){
		case	0:
			*pbData = (BYTE)dwData;
		break;
		case	1:
			*pbData = (BYTE)(dwData >> 8);
		break;
		case	2:
			*pbData = (BYTE)(dwData >> 16);
		break;
		case	3:
			*pbData = (BYTE)(dwData >> 24);
		break;
        default :
            return FALSE;
	}
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}
//===================================================================
// 説  明		IO書き込み
// 引  数		wBsn		BSN
//			    wData       16bit Data Write
//              pdwData     書き込み失敗時、FF000000h を返します。
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwSetIoWrite( WORD wBsn,WORD wData ,PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	bln = (*pCTDSetIoWrite)(wBsn,wData,pdwData);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetIoWrite wBsn= %d wData= 0x%x pdwData= 0x%x \n",wBsn, wData, *pdwData);
#else
	sprintf_s((char*)buffx, 500, "SetIoWrite wBsn= %d wData= 0x%x pdwData= 0x%x \n",wBsn, wData, *pdwData);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return bln;
}
//===================================================================
//badr =0:0byte 1:1byte
BOOL APIENTRY CTDwSetIoWriteByte(WORD wBsn,BYTE badr,BYTE bData)
{
DWORD dwData;
WORD  w;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	switch(badr){
		case	0:
				w = (WORD)bData;
                BackIO &= 0xff00;
                BackIO |= w;
		break;
		case	1:

				w = (WORD)(bData<<8);
                BackIO &= 0x00ff;
                BackIO |= w;
		break;
        default :
            return FALSE;
	}

	if(CTDwSetIoWrite(wBsn,BackIO,&dwData) == FALSE)
			return FALSE;
	if(dwData == 0xff000000)
			return FALSE;


	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}



//---------------------------------------------------------------------
//				速度関連機能
//---------------------------------------------------------------------

//====================================================================
//		    説明	速度パラメーター
//			引数：	wBsn	=BSN番号
//					wAxis	=制御軸
//					wLospd	=START/STOP SPEED
//					wHiSpd	=OBJECT SPEED
//					wRange	=Range Data
//					wRate	=RATE1
//					wSrate  =SW1
//					STorSW  =直線モード　or S字モード
//
//			戻り値：TRUE	=成功
//					FALSE	=失敗
//====================================================================

BOOL SetSpd(WORD wBsn, WORD wAxis, WORD wLoSpd, WORD wHiSpd, WORD wRange, WORD wRate,
				   WORD wSRate, short STorSW)
{
	/* 設定 */
	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_RANGE_WRITE, wRange)) return FALSE;

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, wLoSpd)) return FALSE;

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, wHiSpd)) return FALSE;

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_RATE1_DATA_WRITE, wRate)) return FALSE;

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_RATE2_DATA_WRITE, 8191)) return FALSE;

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_SW2_DATA_WRITE, 4095)) return FALSE;

	if (STorSW == CTD_STRAIGHT_MOVE){
		if (FALSE == CTDwCommandWrite(wBsn, wAxis, CTD_STRAIGHT_ACCELERATE_MODE_SET)) return FALSE;
	}else{
		if (FALSE == CTDwCommandWrite(wBsn, wAxis, CTD_S_CURVE_ACCELERATE_MODE_SET)) return FALSE;
	}

	if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_SW1_DATA_WRITE, wSRate)) return FALSE;

	return TRUE;
}

//====================================================================
//			説明　　速度パラメーター設定
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//					dLowSpeed	=開始速度(PPS)
//					dHighSpeed  =最高速度(PPS)
//					sAccTime	=加減速時間(msec)
//					dSRate		=S字加減率(%)
//
//			戻り値: TRUE		=成功
//					FALSE		=失敗
//					CAUTION
//====================================================================

BOOL APIENTRY CTDwSpeedParameterWrite(WORD wBsn, WORD wAxis, double dLowSpeed, double dHighSpeed,
									  short sAccTime, double dSRate)
{
	double	dBuf, dMaxSpd, dRng, dHiSpd, dLoSpd, dSpd, dRate, dSw, tim, sr, st, dAcc, dObjSpeed;
	short	sRet, sBuf, sCtrl = 0;
	long	lngTemp;


	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	sr = 0.0;
	st = 0.0;

	sRet = TRUE;


	if (dSRate == CTD_STRAIGHT_MOVE)
		sBuf = CTD_STRAIGHT_MOVE;
	else
		sBuf = 0;


	/* 開始速度が0で */
	if (dLowSpeed == 0.0){
		/* 最高速度リミットチェック */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
		/* 最高速度が正しく入力できていて */
		/* 加減速度リミットチェック */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
		/* 加減速時間が0の場合 */
		if (sAccTime == 0){
			sCtrl = 1;				/* 長方形制御 */
		/* 加減速時間が正しく入力できていた場合 */
		}else{
			sCtrl = 2;				/* 自起動速度を強制的に1にした台形制御 */
		}
	/* 開始速度が0以上で */
	}else if (dLowSpeed > 0.0){
		/* 最高速度リミットチェック */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
		/* 最高速度が正しく入力できていて */
		/* 開始速度と最高速度が同じ場合 */
		if (dLowSpeed == dHighSpeed){
			sCtrl = 1;				/* 長方形制御 */
		/* 開始速度と加減速度が違う場合 */
		}else{
			sCtrl = 5;				/* 通常制御 */
		}

	}

	/* 加減速時間が0で */
	if (sAccTime == 0){
		/* 最高速度が正しく入力できていて */
		/* 加減速度リミットチェック */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
		/* 開始速度より最高速度が大きな値の場合 */
		if (dLowSpeed <= dHighSpeed){
			sCtrl = 1;				/* 長方形制御 */
		/* 正しく入力できていなかったらエラー */
		}else{
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
	}
	//USPG28 or USPG48
	dObjSpeed = SpdParam[wBsn].dObjSpd[wAxis];

	/* 長方形制御 */
	if (sCtrl == 1){
		/* 最高速度リミットチェック */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* レンジ計算 */
		dRng = 8120000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* レンジリミットチェック */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* 算出されたレンジで最高の周波数を求める */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* 目標 */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* 目標速度リミットチェック */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* 時起動速度に最高速度の値を代入 */
		dLoSpd = dHiSpd;

		/* レート、Sレートを強制的に1を書く。意味無しパラメータ */
		dRate = 1.0;
		dSw = 1.0;

		/* 速度設定 */
		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
						   (WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return TRUE;
	/* 台形制御：強制的に自起動操作 */
	}else if (sCtrl == 2){
		/* 強制的に自起動を1にする */
		dLoSpd = 1;

		/* 最高速度リミットチェック */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 加減速度リミットチェック */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 加減率リミットチェック */
		if ((dSRate < -1.0) || (dSRate > 100.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* レンジ計算 */
		dRng = 8190000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* レンジリミットチェック */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* 算出されたレンジで最高の周波数を求める */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* 目標 */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* 目標速度リミットチェック */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* 最高速度と開始速度の差算出 */
		dSpd = dHiSpd - dLoSpd;

		/* 絶対値に変換 */
		if (dSpd < 0.0) dSpd *= -1.0;

		if (dSpd == 0.0){
			dRate = 1.0;
		}else{
			dAcc = (double)sAccTime;
			dAcc /= 1000.0;
			tim = dSRate / 100.0;
			sr = (dAcc * tim) / 2.0;
			st = dAcc * (1.0 - tim);
			dRate = ((sr + st) * (double)BAS_RTF) / (dMaxSpd - dLoSpd);
		}

		/* レートリミットチェック */
		if (dRate < 1.0){
			dRate = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dRate > 8190.0){
			dRate = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* S字区間速度データ計算 */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* S字区間リミットチェック */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (dLoSpd > dHiSpd)
			dLoSpd = dHiSpd;

		/* 設定 */
		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
						   (WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return sRet;	//2015.09.16 return TRUEをsRetに修正 beppu
	}else if(sCtrl == 5){
		/* 開始速度最低リミットチェック */
		if ((dLowSpeed <= 0.0) || (dLowSpeed > 4095000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 最高速度リミットチェック */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8191000.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 開始速度が最高速度よりも大きな値だったら */
		if (dHighSpeed < dLowSpeed){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 加減速度リミットチェック */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 加減率リミットチェック */
		if ((dSRate < -1.0) || (dSRate > 100.0)){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* レンジ計算 */
		dRng = 8190000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* レンジリミットチェック */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* 算出されたレンジで最高の周波数を求める */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* 目標 */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* 目標速度リミットチェック */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* 開始速度データ算出 */
		dLoSpd = dLowSpeed * (dRng / (double)BAS_RNG);

		/* 開始速度リミットチェック */
		if (dLoSpd < 1.0){
			dLoSpd = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dLoSpd > 8190.0){
			dLoSpd = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* 最高速度と開始速度の差算出 */
		dSpd = dHiSpd - dLoSpd;

		/* 絶対値に変換 */
		if (dSpd < 0.0) dSpd *= -1.0;

		if (dSpd == 0.0){
			dRate = 1.0;
		}else{
			dAcc = (double)sAccTime;
			dAcc /= 1000.0;
			tim = dSRate / 100.0;
			sr = (dAcc * tim) / 2.0;
			st = dAcc * (1.0 - tim);
			dRate = ((sr + st) * (double)BAS_RTF) / (dMaxSpd - dLoSpd);
		}

		/* レートリミットチェック */
		if (dRate < 1.0){
			dRate = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dRate > 8190.0){
			dRate = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* S字区間速度データ計算 */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* S字区間リミットチェック */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (dLoSpd > dHiSpd)
			dLoSpd = dHiSpd;

		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
			(WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){

			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return sRet;
	}
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return FALSE;
}

//===================================================================
//			説明　　速度パラメーター設定
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//					dObjSpeed	=移動速度(%)
//
//			戻り値: TRUE		=成功
//					FALSE		=失敗
//					CAUTION
//===================================================================

BOOL APIENTRY CTDwSpeedWrite(WORD wBsn, WORD wAxis, double dObjSpeed)
{
	WORD	wBuf;
	double	dHiSpd, dLoSpd, dObjSpd, dRng, dSw, tim, sr, st, dBuf, dAcc;
	short	sRet = TRUE;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	/* リミットチェック */
	if ((dObjSpeed <= 0.0) || (dObjSpeed > 100.0)){
		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return FALSE;
	}

	/* Object Speedを取得 */
	if (FALSE == CTDwDataHalfRead(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_READ, &wBuf)){
		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return FALSE;
	}
	dObjSpd = (double)wBuf;

	/* 開始速度データ算出 */
	if (FALSE == CTDwDataHalfRead(wBsn, wAxis, CTD_RANGE_READ, &wBuf)){
		CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
		return FALSE;
	}
	dRng = (double)wBuf;

	/* 算出されたレンジで最高の周波数を求める */
	/* 目標 */
	dHiSpd = SpdParam[wBsn].dHiSpd[wAxis] * (dObjSpeed / 100.0);	//USPG28 or USPG48

	dHiSpd *= (dRng / (double)BAS_RNG);

	/* 目標速度リミットチェック */
	if (dHiSpd < 1.0) dHiSpd = 1.0;
	if (dHiSpd > 8190.0) dHiSpd = 8190.0;

	dLoSpd = SpdParam[wBsn].dLoSpd[wAxis] * (dRng / (double)BAS_RNG);	//USPG28 or USPG48

	/* 開始速度リミットチェック */
	if (dLoSpd < 1.0){
		dLoSpd = 1.0;
		sRet = CTD_CAUTION;
	}
	if (dLoSpd > 8190.0){
		dLoSpd = 8190.0;
		sRet = CTD_CAUTION;
	}

	if (dLoSpd > dHiSpd){
		dLoSpd = dHiSpd;
		sRet = CTD_CAUTION;
	}
	//USPG28 or USPG48
	if (dObjSpeed < SpdParam[wBsn].dObjSpd[wAxis]){ 
		/* 最低速度設定 */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, (WORD)(dLoSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 最高速度設定 */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, (WORD)(dHiSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
	}else if(dObjSpeed > SpdParam[wBsn].dObjSpd[wAxis]){
		/* 最高速度設定 */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, (WORD)(dHiSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}

		/* 最低速度設定 */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, (WORD)(dLoSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
	}
	//USPG28 or USPG48
	if (SpdParam[wBsn].dSCurve[wAxis] != 0){

		dAcc = (double)SpdParam[wBsn].sUpDnSpd[wAxis];
		dAcc /= 1000.0;
		tim = SpdParam[wBsn].dSCurve[wAxis] / 100.0;
		sr = (dAcc * tim) / 2.0;
		st = dAcc * (1.0 - tim);

		/* S字区間速度データ計算 */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* リミットチェック */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_SW1_DATA_WRITE, (WORD)(dSw + 0.5))){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
		}
	}

	SpdParam[wBsn].dObjSpd[wAxis] = dObjSpeed;

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return sRet;
}

//====================================================================
//			説明　　速度パラメーター設定
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			
//			戻り値:
//					*pdLowSpeed	=開始速度(PPS)
//					*pdHighSpeed=最高速度(PPS)
//					*psAccTime	=加減速時間(msec)
//					*pdSRate	=S字加減率(%)
//					TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL  APIENTRY  CTDwSpeedParameterRead(WORD wBsn, WORD wAxis, double *pdLowSpeed, double *pdHighSpeed,
									 short *psAccTime, double *pdSRate)
{

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	*pdLowSpeed = SpdParam[wBsn].dLoSpd[wAxis];
	*pdHighSpeed = SpdParam[wBsn].dHiSpd[wAxis];
	*psAccTime = SpdParam[wBsn].sUpDnSpd[wAxis];
	*pdSRate = SpdParam[wBsn].dSCurve[wAxis];

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}

//====================================================================
//			説明　　速度パラメーター設定
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			戻り値:
//					*pdObjSpeed	=移動速度(%)
//				 	TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL APIENTRY CTDwSpeedRead(WORD wBsn, WORD wAxis, double *pdObjSpeed)
{

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	*pdObjSpeed = SpdParam[wBsn].dObjSpd[wAxis];

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}


//====================================================================
//			説明　　インターナルカウンター読み込み
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			戻り値:
//					*pdwData	=インターナルカウンター
//				 	TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL APIENTRY CTDwGetInternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    if( CTDwDataFullRead(wBsn,wAxis,0x29,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}

//====================================================================
//			説明　　エクスターナルカウンター読み込み
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			戻り値:
//					*pdwData	=エクスターナルカウンター
//				 	TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL  APIENTRY CTDwGetExternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    if( CTDwDataFullRead(wBsn,wAxis,0x2d,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}
//====================================================================
//			説明　　ドライブパルスカウンター読み込み
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			戻り値:
//					*pdwData	=ドライブパルスカウンター
//				 	TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL APIENTRY CTDwGetDrivePulseCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
 	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    if( CTDwDataFullRead(wBsn,wAxis,0x13,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}
//====================================================================
//			説明　　NOW SPEED DATA 読み込み
//			引数：	wBSN		=BSN番号
//					wAxis		=制御軸
//			戻り値:
//					*pwData		=NOW SPEED DATA
//				 	TRUE		=成功
//					FALSE		=失敗
//====================================================================

BOOL APIENTRY CTDwGetNowSpeedData(WORD wBsn, WORD wAxis, PWORD pwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    if( CTDwDataHalfRead(wBsn,wAxis,0x12,pwData) == FALSE){
			CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
			return FALSE;
	}

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}


//===================================================================
// 説  明		CALL BACK 関数
// 引  数		HANDLE	:hMutex
//				void (WINAPI *lpfnHandler)(void)
//						:CALL BACK関数アドレス
// 戻り値		0=	成功
//			   -1=	失敗
//===================================================================
int APIENTRY CTDwCallBack(HANDLE hMutex,void (WINAPI *lpfnHandler)(void))
{
int bln;

    if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    bln = (*pCTDCallBack)(hMutex,lpfnHandler);

#if _DEBUG 
	DebugPrint("CallBack \n");
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
    return bln;
}
//===================================================================
// 説  明		CALL BACK 開放
// 戻り値		0=	成功
//			   -1=	失敗
//===================================================================
int APIENTRY CTDwFreeCallBack(void)
{
int bln;

    if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
    bln = ( *pCTDFreeCallBack)();

#if _DEBUG 
	DebugPrint("FreeCallBack \n");
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
    return bln;
}

//==============================================================================
//					PMCコマンドデータ書き込み
//				引数：
//					wBsn	=BSN番号
//					wAxis	=軸
//					bCmd    =PMCコマンド
//					dwData  =書き込みデータ
//					dwStepNo=ステップNo.
//				戻り値：
//					TRUE or FALSE
//===============================================================================

BOOL APIENTRY CTDwPDataWrite(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPDataWrite)(wBsn,wAxis,bCmd,dwData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PDataWrite wBsn= %d wAxis= %d bCmd= %d dwData= %d dwStepNo= %d \n",
						wBsn, wAxis, bCmd, dwData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					データリード
//				引数：
//					wBsn	  =BSN番号
//					wAxis	  =軸
//					bCmd      =PMCコマンド
//					dwANDData =読み込みデータとANDし真であれば次ステップ
//					dwStepNo  =ステップNo.
//				戻り値：
//					TRUE or FALSE
//===============================================================================

BOOL APIENTRY CTDwPDataRead(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwANDData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPDataRead)(wBsn,wAxis,bCmd,dwANDData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PDataRead wBsn= %d wAxis= %d bCmd= %d dwANDData= %d dwStepNo= %d \n",
						wBsn, wAxis, bCmd, dwANDData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}

//==============================================================================
//					Drive データリード
//				引数：
//					wBsn	  =BSN番号
//					wAxis	  =軸
//					bSetData  =0bit:ON 1軸ドライブ終了
//							   1bit:ON 2軸ドライブ終了
//							   2bit:ON 3軸ドライブ終了
//							   3bit:ON 4軸ドライブ終了
//							   4bit:ON 補間割り込み
//							　　このSetDataが一致したら次ステップ
//					dwStepNo  =ステップNo.
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPDriveRead(WORD wBsn,WORD wAxis,BYTE bSetData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPDriveRead)(wBsn,wAxis,bSetData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "POutPort wBsn= %d wAxis= %d bSetData= %d dwStepNo= %d \n",
						wBsn, wAxis, bSetData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}

//==============================================================================
//					PMC Port書き込み
//				引数：
//					wBsn	  =BSN番号
//					wAxis	  =軸
//					bAdr      =Port Address
//					bdata     =書き込みデータ
//					dwStepNo  =ステップNo.
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPOutPort(WORD wBsn,WORD wAxis,BYTE bAdr,BYTE bdata, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPOutPort)(wBsn,wAxis,bAdr,bdata,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "POutPort wBsn= %d wAxis= %d bAdr= %d bdata= %d dwStepNo= %d \n",
						wBsn, wAxis, bAdr, bdata, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					PMC Port読み込み
//				引数：
//					wBsn	  =BSN番号
//					wAxis	  =軸
//					bIoAdr    =Port Address
//					bCmpData  =比較データ
//					bActiveLevel =bCmpData比較した結果、次ステップへ移行するアクティブレベル
//					bCondition =0:条件一致で次ステップ。条件が一致しない場合、条件が一致するまでwait
//								1:条件一致で次ステップ。条件が一致しない場合、dwJmpStepNoへジャンプ
//					dwJmpStep  =ジャンプ先ステップNo.
//					dwStepNo  =ステップNo.
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPInPort(WORD wBsn,WORD wAxis,BYTE bIoAdr,BYTE bCmpData,BYTE bActiveLevel,BYTE bCondition,DWORD dwJmpStepNo, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPInPort)(wBsn,wAxis,bIoAdr,bCmpData,bActiveLevel,bCondition,dwJmpStepNo,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PInPort wBsn= %d wAxis= %d bIoAdr= %d bCmpData= %d bActiveLevel= %d bCondition=%d dwJmpStepNo=%d dwStepNo=%d \n",
						wBsn, wAxis, bIoAdr, bCmpData, bActiveLevel, bCondition, dwJmpStepNo, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					プログラム動作開始・終了
//				引数：
//					wBsn	=BSN番号
//					bSS		=1:プログラム開始 0:プログラム終了
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGStartStop(WORD wBsn,BYTE bSS  )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPGStartStop)(wBsn,bSS) == FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
				
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGStartStop wBsn= %d bSS=%d\n",wBsn, bSS);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					プログラム総ステップ数
//				引数：
//					wBsn	=BSN番号
//					dwStepNo=総ステップ数
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGAllStep(WORD wBsn,DWORD dwStepNo  )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPGAllStep)(wBsn,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
				
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGAllStep wBsn= %d dwStepNo=%d\n",wBsn, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					Wait時間
//				引数：
//					wBsn	  =BSN番号
//					dwWaitTime=Wait時間
//					dwStepNo  =ステップ番号
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGWait(WORD wBsn,DWORD dwWaitTime,DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPGWait)(wBsn,dwWaitTime,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
		
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGWait wBsn= %d dwWaitTime=%d dwStepNo=%d\n",wBsn, dwWaitTime, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					プログラム終了
//				引数：
//					wBsn	  =BSN番号
//					dwStepNo  =終了ステップ番号
//				戻り値：
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGEnd(WORD wBsn,DWORD dwStepNo)
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPGEnd)(wBsn,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
			
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGEnd wBsn= %d dwStepNo=%d\n",wBsn, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//==============================================================================
//					プログラム Status
//				引数：
//					wBsn	  =BSN番号
//				戻り値：
//					dwStepNo  =実行中ステップ番号
//					pbStatePg =0:NO RUN 1:RUNNING
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGStatus(WORD wBsn,PDWORD pdwStepNo, PBYTE pbStatePg)
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDPGStatus)(wBsn,pdwStepNo,pbStatePg)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
			
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGStatus wBsn= %d pdwStepNo=%d pbStatePg=%d\n",wBsn, *pdwStepNo, *pbStatePg);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;

}

//===================================================================
// 説  明		スタート信号の制御
// 引  数		wBsn		BSN
//				wAxis1Start	制御軸１,２の同期スタート信号
//							1軸 ON =1 OFF =0 (0bit)
//						    2軸 ON =1 OFF =0 (1bit)
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwStartSignalWrite(WORD wBsn, WORD wStart)
{
	if ( hDLL == NULL ) return FALSE;
	

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDStartSignalWrite)(wBsn,wStart)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}
//===================================================================
// 説  明		全軸のステータスを取得
// 引  数		wBsn	BSN
//				pbData	読み込んだデータを格納する領域へのポインタ
// 戻り値		TRUE	成功
//				FALSE	失敗
//===================================================================
BOOL APIENTRY CTDwGetAxisStatus( WORD wBsn, PBYTE pbData )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDGetAxisStatus)(wBsn,pbData)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}
//===================================================================
// 説  明		指定軸の各ステータスを取得
// 引  数		wBsn		BSN
//				wAxis		対象制御軸
//				pdwData		読み込んだデータを格納する領域へのポインタ
// 戻り値		TRUE		成功
//				FALSE		失敗
//===================================================================
BOOL APIENTRY CTDwGetAxisAllPort( WORD wBsn, WORD wAxis, PDWORD pdwData )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	if((*pCTDGetAxisAllPort)(wBsn,wAxis,pdwData)== FALSE){
				CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return TRUE;
}



//API関数

//===================================================================
// 説  明		アラーム停止機能の有効/無効設定、リミット入力停止設定、INPOSITION WAIT MODE設定、ALARM STOP ENABLE MODE設定
// 引  数		wBsn			対象ＢＳＮ
//				wAxis			対象制御軸
//				bControl		設定データ
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwSetControl( WORD wBsn, WORD wAxis, BYTE bControl )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSetControl)(wBsn,wAxis,bControl);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetControl wBsn=%d wAxis=%d bControl=0x%x Result=0x%x \n",wBsn,wAxis,bControl,Result);
#else
	sprintf_s((char*)buffx, 500, "SetControl wBsn=%d wAxis=%d bControl=0x%x Result=0x%x \n",wBsn,wAxis,bControl,Result);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		アラーム停止機能の有効/無効、リミット入力停止、INPOSITION WAIT MODE、ALARM STOP ENABLE MODE読み出し
// 引  数		wBsn			対象ＢＳＮ
//				wAxis			対象制御軸
//				pbControl		読出データ
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetControl( WORD wBsn, WORD wAxis, PBYTE pbControl )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetControl)(wBsn,wAxis,pbControl);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetControl wBsn=%d wAxis=%d pbControl=0x%x Result=0x%x \n",wBsn,wAxis,*pbControl,Result);
#else
	sprintf_s((char*)buffx, 500, "GetControl wBsn=%d wAxis=%d pbControl=0x%x Result=0x%x \n",wBsn,wAxis,*pbControl,Result);
#endif

	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		ソフトリミット設定
// 引  数		wBsn			ＢＳＮ
//				wAxis			対象制御軸
//				dwHighLimit		上限位置
//				dwLowLimit		下限位置
//				wMode			ソフトリミット機能の有効、無効の設定。
//								0：両側ソフトリミット有効
//								1：上限位置ソフトリミット無効、下限位置ソフトリミット有効
//								2：上限位置ソフトリミット有効、下限位置ソフトリミット無効
//								3：両側ソフトリミット無効
//				wEvent			出力処理設定
//								0：該当軸が減速停止
//								1：該当軸が急停止
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwSetSoftLimit( WORD wBsn, WORD wAxis, DWORD dwHighLimit, DWORD dwLowLimit, WORD wMode, WORD wEvent )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSetSoftLimit)(wBsn,wAxis,dwHighLimit,dwLowLimit,wMode,wEvent);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetSoftLimit wBsn=%d wAxis=%d dwHighLimit=%d dwLowLimit=%d wMode=%d wEvent=%d Result=0x%x \n",wBsn,wAxis,dwHighLimit,dwLowLimit,wMode,wEvent,Result);
#else
	sprintf_s((char*)buffx, 500, "SetSoftLimit wBsn=%d wAxis=%d dwHighLimit=%d dwLowLimit=%d wMode=%d wEvent=%d Result=0x%x \n",wBsn,wAxis,dwHighLimit,dwLowLimit,wMode,wEvent,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		ソフトリミット取得
// 引  数		wBsn			ＢＳＮ
//				wAxis			対象制御軸
//				pdwHighLimit	上限位置
//				pdwLowLimit		下限位置
//				pwMode			ソフトリミット機能の有効、無効  
//								0：両側ソフトリミット有効
//								1：上限位置ソフトリミット無効、下限位置ソフトリミット有効
//								2：上限位置ソフトリミット有効、下限位置ソフトリミット無効
//								3：両側ソフトリミット無効
//				pwEvent			出力処理
//								0：該当軸が減速停止
//								1：該当軸が急停止
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetSoftLimit( WORD wBsn, WORD wAxis, PDWORD pdwHighLimit, PDWORD pdwLowLimit, PWORD pwMode, PWORD pwEvent )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetSoftLimit)(wBsn,wAxis,pdwHighLimit,pdwLowLimit,pwMode,pwEvent);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetSoftLimit wBsn=%d wAxis=%d pdwHighLimit=%d pdwLowLimit=%d pwMode=%d pwEvent=%d Result=0x%x \n",wBsn,wAxis,*pdwHighLimit,*pdwLowLimit,*pwMode,*pwEvent,Result);
#else
	sprintf_s((char*)buffx, 500, "GetSoftLimit wBsn=%d wAxis=%d pdwHighLimit=%d pdwLowLimit=%d pwMode=%d pwEvent=%d Result=0x%x \n",wBsn,wAxis,*pdwHighLimit,*pdwLowLimit,*pwMode,*pwEvent,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		DLL,ドライババージョン読み出し
// 引  数		wBsn			ＢＳＮ
//				pbDLLVersion	DLLバージョン情報を格納する領域へのポインタ
//				pbDrvVersion	ドライババージョン情報を格納する領域へのポインタ
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetVersion( PBYTE pbDLLVersion, PBYTE pbDrvVersion ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetVersion)(pbDLLVersion,pbDrvVersion);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetVersion pbDLLVersion=%d pbDrvVersion=%d Result=0x%x \n", *pbDLLVersion,*pbDrvVersion,Result);
#else
	sprintf_s((char*)buffx, 500, "GetVersion pbDLLVersion=%d pbDrvVersion=%d Result=0x%x %d \n", *pbDLLVersion,*pbDrvVersion,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		USPG-28Lのステータスを取得
// 引  数		wBsn			ＢＳＮ
//				wAxis			対象制御軸
//				pbStatus		PXPG-48/PXPG-88のステータスを格納する領域へのポインタ	
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetBoardStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus )
{
WORD Result;
	if (hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetBoardStatus)(wBsn,wAxis,pbStatus);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetBoardStatus wBsn=0x%x wAxis=%d pbStatus0=0x%x pbStatus1=0x%x pbStatus2=0x%x Result=0x%x \n", wBsn,wAxis,*(pbStatus+0),*(pbStatus+1),*(pbStatus+2),Result);
#else
	sprintf_s((char*)buffx, 500, "GetBoardStatus wBsn=0x%x wAxis=%d pbStatus0=0x%x pbStatus1=0x%x pbStatus2=0x%x Result=0x%x \n", wBsn,wAxis,*(pbStatus+0),*(pbStatus+1),*(pbStatus+2),Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
//			各種カウンタの現在カウンタ値を取得
// 引  数	wBsn				ＢＳＮ
//			wAxis				対象制御軸
//			bMode				0:出力パルスカウンタ
//								1:未使用
//								2:ドライブパルスカウンタ
//								3:未使用
//			pdwData				指定カウンタの値を格納する領域へのポインタ。
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetCounter( WORD wBsn, WORD wAxis, BYTE bMode, PDWORD pdwData )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetCounter)(wBsn,wAxis,bMode,pdwData);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetCounter wBsn=0x%x wAxis=%d bMode=%d pdwData=%d Result=0x%x \n", wBsn,wAxis,bMode,*pdwData,Result);
#else
	sprintf_s((char*)buffx, 500, "GetCounter wBsn=0x%x wAxis=%d bMode=%d pdwData=%d Result=0x%x \n", wBsn,wAxis,bMode,*pdwData,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		スピードパラメータ設定。
// 引  数		wBsn		ＢＳＮ
//				wAxis		対象制御軸
//				pSPDData	スピードパラメータデータを格納する領域へのポインタ
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwSetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSetSpeedParameter)(wBsn,wAxis,pSPDData);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetSpeedParameter wBsn=0x%x wAxis=%d Mode=%d LowSpd=%d HighSpd=%d "
		"UPTime=%d DWTime=%d UPSRate=%d DWSRate=%d bOverONOFF=%d Result=0x%x \n",wBsn,wAxis,pSPDData->dwMode,pSPDData->dwLowSpeed,
		pSPDData->dwHighSpeed,pSPDData->dwUPTime,pSPDData->dwDWTime,pSPDData->dwUPSRate,pSPDData->dwDWSRate,pSPDData->bOverONOFF,Result);
#else
	sprintf_s((char*)buffx, 500, "SetSpeedParameter wBsn=0x%x wAxis=%d Mode=%d LowSpd=%d HighSpd=%d "
		"UPTime=%d DWTime=%d UPSRate=%d DWSRate=%d bOverONOFF=%d Result=0x%x \n",wBsn,wAxis,pSPDData->dwMode,pSPDData->dwLowSpeed,
		pSPDData->dwHighSpeed,pSPDData->dwUPTime,pSPDData->dwDWTime,pSPDData->dwUPSRate,pSPDData->dwDWSRate,pSPDData->bOverONOFF,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		スピードパラメータ取得。
// 引  数		wBsn		ＢＳＮ
//				wAxis		対象制御軸
//				pSPDData	スピードパラメータデータを格納する領域へのポインタ
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetSpeedParameter)(wBsn,wAxis,pSPDData);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetSpeedParameter wBsn=0x%x wAxis=%d Mode=%d LowSpd=%d HighSpd=%d "
		"UPTime=%d DWTime=%d UPSRate=%d DWSRate=%d bOverONOFF=%d Result=0x%x \n",wBsn,wAxis,pSPDData->dwMode,pSPDData->dwLowSpeed,
		pSPDData->dwHighSpeed,pSPDData->dwUPTime,pSPDData->dwDWTime,pSPDData->dwUPSRate,pSPDData->dwDWSRate,pSPDData->bOverONOFF,Result);
#else
	sprintf_s((char*)buffx, 500, "GetSpeedParameter wBsn=0x%x wAxis=%d Mode=%d LowSpd=%d HighSpd=%d "
		"UPTime=%d DWTime=%d UPSRate=%d DWSRate=%d bOverONOFF=%d Result=0x%x \n",wBsn,wAxis,pSPDData->dwMode,pSPDData->dwLowSpeed,
		pSPDData->dwHighSpeed,pSPDData->dwUPTime,pSPDData->dwDWTime,pSPDData->dwUPSRate,pSPDData->dwDWSRate,pSPDData->bOverONOFF,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		ドライブ実行中、目標速度の変更
// 引  数		wBsn		対象ＢＳＮ
//				wAxis		対象制御軸
//				dwHighSpeed	変更した後の目標速度
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwSpeedOverride( WORD wBsn, WORD wAxis, DWORD dwHighSpeed )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSpeedOverride)(wBsn,wAxis,dwHighSpeed);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SpeedOverride wBsn=0x%x wAxis=%d dwHighSpeed=%d Result=0x%x \n", wBsn,wAxis,dwHighSpeed,Result);
#else
	sprintf_s((char*)buffx, 500, "SpeedOverride wBsn=0x%x wAxis=%d dwHighSpeed=%d Result=0x%x \n", wBsn,wAxis,dwHighSpeed,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		現在速度を取得
// 引  数		wBsn			対象ＢＳＮ
//              wAxis			対象制御軸
//              pdwNowSpeed		現在速度を格納する領域へのポインタ。
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwGetNowSpeed( WORD wBsn, WORD wAxis, PDWORD pdwNowSpeed )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetNowSpeed)(wBsn,wAxis,pdwNowSpeed);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetNowSpeed wBsn=0x%x wAxis=%d pdwNowSpeed=%d Result=0x%x \n", wBsn,wAxis,*pdwNowSpeed,Result);
#else
	sprintf_s((char*)buffx, 500, "GetNowSpeed wBsn=0x%x wAxis=%d pdwNowSpeed=%d Result=0x%x \n", wBsn,wAxis,*pdwNowSpeed,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		ドライブ実行
// 引  数		wBsn	対象ＢＳＮ
//              wAxis	対象制御軸
//              bMode	ドライブの種類を指定
// 						0:相対位置 位置決めドライブ
// 						1:絶対位置 位置決めドライブ
// 						2:+方向連続ドライブ
//						3:-方向連続ドライブ　
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwDriveStart( WORD wBsn, WORD wAxis, BYTE bMode, DWORD dwPulse )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDDriveStart)(wBsn,wAxis,bMode,dwPulse);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DriveStart wBsn=0x%x wAxis=%d bMode=%d dwPulse=%d Result=0x%x \n", wBsn,wAxis,bMode,dwPulse,Result);
#else
	sprintf_s((char*)buffx, 500, "DriveStart wBsn=0x%x wAxis=%d bMode=%d dwPulse=%d Result=0x%x \n", wBsn,wAxis,bMode,dwPulse,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		ドライブ停止
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              bStop		0=減速停止 1=急停止
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwDriveStop( WORD wBsn, WORD wAxis, BYTE bStop )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDDriveStop)(wBsn,wAxis,bStop);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "DriveStop wBsn=0x%x wAxis=%d bStop=%d Result=0x%x \n", wBsn,wAxis,bStop,Result);
#else
	sprintf_s((char*)buffx, 500, "DriveStop wBsn=0x%x wAxis=%d bStop=%d Result=0x%x \n", wBsn,wAxis,bStop,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		移動量のオーバーライドを行います。
// 引  数		wBsn		ＢＳＮ
//				wAxis		対象制御軸
//				dwPulse		位置決めドライブの時、新しい位置座標を指定します。
//				       		設定範囲：80000000h～7FFFFFFFh(－２１４７４８３６４８～＋２１４７４８３６４７) 
//			         		単    位：PPS
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwPulseOverride( WORD wBsn, WORD wAxis, DWORD dwPulse )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDPulseOverride)(wBsn,wAxis,dwPulse);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "PulseOverride wBsn=0x%x wAxis=%d dwPulse=%d Result=0x%x \n", wBsn,wAxis,dwPulse,Result);
#else
	sprintf_s((char*)buffx, 500, "PulseOverride wBsn=0x%x wAxis=%d dwPulse=%d Result=0x%x \n", wBsn,wAxis,dwPulse,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		多軸同期制御設定。
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              bONOFF		0 = 待機    1 = 実行
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwSyncONOFF( WORD wBsn, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSyncONOFF)(wBsn,bONOFF);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SyncONOFF wBsn=0x%x bONOFF=%d Result=0x%x \n", wBsn,bONOFF,Result);
#else
	sprintf_s((char*)buffx, 500, "SyncONOFF wBsn=0x%x bONOFF=%d Result=0x%x \n", wBsn,bONOFF,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		原点復帰
// 引  数		wBsn		ＢＳＮ
//              wAxis		対象制御軸
//　　　　　　　bOrgMode	上位4bit:原点復帰方向  下位4bit:原点復帰モード
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwORGSignalStart( WORD wBsn, WORD wAxis, BYTE bOrgMode )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDORGSignalStart)(wBsn,wAxis,bOrgMode);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ORGSignalStart wBsn=0x%x wAxis=%d bOrgMode=%d Result=0x%x \n", wBsn,wAxis,bOrgMode,Result);
#else
	sprintf_s((char*)buffx, 500, "ORGSignalStart wBsn=0x%x wAxis=%d bOrgMode=%d Result=0x%x \n", wBsn,wAxis,bOrgMode,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		原点復帰中に、強制的に停止指令を発行。
// 引  数		wBsn		ＢＳＮ
//              wAxis		対象制御軸
//　　　　　　　bStop		原点復帰中に当関数が行われ、原点復帰を強制終了する場合、停止方法設定
//							0 = 減速停止     1 = 急停止
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwORGSignalStop( WORD wBsn, WORD wAxis, BYTE bStop )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDORGSignalStop)(wBsn,wAxis,bStop);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ORGSignalStop wBsn=0x%x wAxis=%d bStop=%d Result=0x%x \n", wBsn,wAxis,bStop,Result);
#else
	sprintf_s((char*)buffx, 500, "ORGSignalStop wBsn=0x%x wAxis=%d bStop=%d Result=0x%x \n", wBsn,wAxis,bStop,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}
	
//===================================================================
// 説  明		USPG-28Lの原点復帰状態を取得。
// 引  数		wBsn		ＢＳＮ
//　　　　　　　wAxis		対象制御軸
//　　　　　　　pbStatus	原点復帰の状態を格納する領域へのポインタ。0=原点復帰終了  1=原点復帰中
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwORGGetStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDORGGetStatus)(wBsn,wAxis,pbStatus);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ORGGetStatus wBsn=0x%x wAxis=%d pbStatus=%d Result=0x%x \n", wBsn,wAxis,*pbStatus,Result);
#else
	sprintf_s((char*)buffx, 500, "ORGGetStatus wBsn=0x%x wAxis=%d pbStatus=%d Result=0x%x \n", wBsn,wAxis,*pbStatus,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		座標位置設定
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              dwPosi		現在位置データ   設定範囲：-2,147,483,648 ～ +2,147,483,647 （80000000h ～ 7FFFFFFFh）
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwORGSetPosiData( WORD wBsn, WORD wAxis, DWORD dwPosi )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDORGSetPosiData)(wBsn,wAxis,dwPosi);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ORGSetPosiData wBsn=0x%x wAxis=%d dwPosi=%d Result=0x%x \n", wBsn,wAxis,dwPosi,Result);
#else
	sprintf_s((char*)buffx, 500, "ORGSetPosiData wBsn=0x%x wAxis=%d dwPosi=%d Result=0x%x \n", wBsn,wAxis,dwPosi,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}
	
//===================================================================
// 説  明		DRIVE信号の書き込み
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              bONOFF		0:DRIVE信号 OFF状態
//              			1:DRIVE信号 ON状態
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwSetDriveONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSetDriveONOFF)(wBsn,wAxis,bONOFF);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetDriveONOFF wBsn=0x%x wAxis=%d bONOFF=%d Result=0x%x \n", wBsn,wAxis,bONOFF,Result);
#else
	sprintf_s((char*)buffx, 500, "SetDriveONOFF wBsn=0x%x wAxis=%d bONOFF=%d Result=0x%x \n", wBsn,wAxis,bONOFF,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		DRST信号の書き込み
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              bONOFF		0:DRST信号 OFF状態
//              			1:DRST信号 ON状態
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwSetDrstONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDSetDrstONOFF)(wBsn,wAxis,bONOFF);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "SetDrstONOFF wBsn=0x%x wAxis=%d bONOFF=%d Result=0x%x \n", wBsn,wAxis,bONOFF,Result);
#else
	sprintf_s((char*)buffx, 500, "SetDrstONOFF wBsn=0x%x wAxis=%d bONOFF=%d Result=0x%x \n", wBsn,wAxis,bONOFF,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		アラームコードを取得
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              wValue		0：現在発生中のアラームコード取得
//                          1～14：過去に発生したアラーム履歴を読み出します。14が一番古い履歴です。
//              pbCode		アラームコードを格納する領域へのポインタ（１Byteデータ）　
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwGetALMCode( WORD wBsn, WORD wAxis, WORD wValue, PBYTE pbCode ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetALMCode)(wBsn,wAxis,wValue,pbCode);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetALMCode wBsn=0x%x wAxis=%d wValue=%d pbCode=%d Result=0x%x \n", wBsn,wAxis,wValue,*pbCode,Result);
#else
	sprintf_s((char*)buffx, 500, "GetALMCode wBsn=0x%x wAxis=%d wValue=%d pbCode=%d Result=0x%x \n", wBsn,wAxis,wValue,*pbCode,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		アラームクリア
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              wValue		0：現在発生中のアラームクリア
//                          1：過去に発生したすべてのアラーム履歴クリア
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwALMCLR( WORD wBsn, WORD wAxis, WORD wValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDALMCLR)(wBsn,wAxis,wValue);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ALMCLR wBsn=0x%x wAxis=%d wValue=%d Result=0x%x \n", wBsn,wAxis,wValue,Result);
#else
	sprintf_s((char*)buffx, 500, "ALMCLR wBsn=0x%x wAxis=%d wValue=%d Result=0x%x \n", wBsn,wAxis,wValue,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		パラメータ関連操作
// 引  数		wBsn 			対象ＢＳＮ
//              wAxis			対象制御軸
//              bType			0:パラメータ書込み  1:パラメータ読出し
//              wClassNo		パラメータ分類
//              wParaNo			パラメータNo
//              dwData			パラメータ書込み時、書込みデータ設定
//              pdwValue		パラメータ読出し時、取得したパラメータ値を格納する領域へのポインタ。
//								パラメータ書込み時、｢0｣を返します。
// 戻り値		0				デフォルト値
//				1				正常終了
//				それ以外		エラーコード
//===================================================================
int APIENTRY CTDwParaCont( WORD wBsn, WORD wAxis, BYTE bType, WORD wClassNo, WORD wParaNo, DWORD dwData, PDWORD pdwValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDParaCont)(wBsn,wAxis,bType,wClassNo,wParaNo,dwData,pdwValue);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "ParaCont wBsn=0x%x wAxis=%d bType=%d wClassNo=%d wParaNo=%d dwData=%d(0x%x) pdwValue=%d Result=0x%x \n", wBsn,wAxis,bType,wClassNo,wParaNo,dwData,dwData,*pdwValue,Result);
#else
	sprintf_s((char*)buffx, 500, "ParaCont wBsn=0x%x wAxis=%d bType=%d wClassNo=%d wParaNo=%d dwData=%d(0x%x) pdwValue=%d Result=0x%x \n", wBsn,wAxis,bType,wClassNo,wParaNo,dwData,dwData,*pdwValue,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//===================================================================
// 説  明		モニタ関連操作
// 引  数		wBsn		対象ＢＳＮ
//              wAxis		対象制御軸
//              bkind		1:モニタ関連操作
//              wCmd		ソフトウェアマニュアル参照
//              wValue		ソフトウェアマニュアル参照
//              pdwValue	モニタデータを格納する領域へのポインタ。
// 戻り値		0			デフォルト値
//				1			正常終了
//				それ以外	エラーコード
//===================================================================
int APIENTRY CTDwEvent( WORD wBsn, WORD wAxis, BYTE bKind, WORD wCmd, WORD wValue, PDWORD pdwValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDEvent)(wBsn,wAxis,bKind,wCmd,wValue,pdwValue);
	
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "Event wBsn=0x%x wAxis=%d bKind=%d wCmd=0x%x wValue=%d pdwValue=%d Result=0x%x \n", wBsn,wAxis,bKind,wCmd,wValue,*pdwValue,Result);
#else
	sprintf_s((char*)buffx, 500, "Event wBsn=0x%x wAxis=%d bKind=%d wCmd=0x%x wValue=%d pdwValue=%d Result=0x%x \n", wBsn,wAxis,bKind,wCmd,wValue,*pdwValue,Result);
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}

//14.12.17 Append
BOOL APIENTRY CTDwLock( void )
{
BOOL bln;
	if ( hDLL == NULL ){
		bln = CTD_ERR_NOT_OPEN;
		return FALSE;
	}
	(*pCTLock)();

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "CTDwLock \n");
#else
	sprintf_s((char*)buffx, 500, "CTDwLock \n");
#endif
	return TRUE;
}

BOOL APIENTRY CTDwUnLock( void )
{
BOOL bln;

	if ( hDLL == NULL ){
		bln = CTD_ERR_NOT_OPEN;
		return FALSE;
	}
	(*pCTUnLock)();

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "CTDwUnLock \n");
#else
	sprintf_s((char*)buffx, 500, "CTDwUnLock \n");
#endif
	return TRUE;
}

int APIENTRY CTDwGetStatusCounterALL( WORD wBsn, PBYTE pbDrvStatus, PBYTE pbMecSignal, PBYTE pbUnvSignal, PDWORD pdwInternal, PDWORD pdwExternal )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 保護対象のコードに入る。
	Result = (*pCTDGetStatusCounterALL)(wBsn,pbDrvStatus,pbMecSignal,pbUnvSignal,pdwInternal,pdwExternal);

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));

#if !defined(_MSC_VER) || _MSC_VER < 1400
	sprintf((char*)buffx, "GetStatusCounterALL wBsn=0x%x, pbDrvStatus[0]=%d pbDrvStatus[1]=%d pbDrvStatus[2]=%d pbDrvStatus[3]=%d "
		"pbMecSignal[0]=%d pbMecSignal[1]=%d pbMecSignal[2]=%d pbMecSignal[3]=%d pbUnvSignal[0]=%d pbUnvSignal[1]=%d pbUnvSignal[2]=%d pbUnvSignal[3]=%d "
		"pdwInternal[0]=$d pdwInternal[1]=%d pdwInternal[2]=%d pdwInternal[3]=%d pdwExternal[0]=%d pdwExternal[1]=%d pdwExternal[2]=%d pdwExternal[3]=%d ",
		*(pbDrvStatus+0),*(pbDrvStatus+1),*(pbDrvStatus+2),*(pbDrvStatus+3),*(pbMecSignal+0),*(pbMecSignal+1),*(pbMecSignal+2),*(pbMecSignal+3),
		*(pbUnvSignal+0),*(pbUnvSignal+1),*(pbUnvSignal+2),*(pbUnvSignal+3),*(pdwInternal+0),*(pdwInternal+1),*(pdwInternal+2),*(pdwInternal+3),
		*(pdwExternal+0),*(pdwExternal+1),*(pdwExternal+2),*(pdwExternal+3));
#else
	sprintf_s((char*)buffx, 500, "GetStatusCounterALL wBsn=0x%x, pbDrvStatus[0]=%d pbDrvStatus[1]=%d pbDrvStatus[2]=%d pbDrvStatus[3]=%d "
		"pbMecSignal[0]=%d pbMecSignal[1]=%d pbMecSignal[2]=%d pbMecSignal[3]=%d pbUnvSignal[0]=%d pbUnvSignal[1]=%d pbUnvSignal[2]=%d pbUnvSignal[3]=%d "
		"pdwInternal[0]=$d pdwInternal[1]=%d pdwInternal[2]=%d pdwInternal[3]=%d pdwExternal[0]=%d pdwExternal[1]=%d pdwExternal[2]=%d pdwExternal[3]=%d ",
		*(pbDrvStatus+0),*(pbDrvStatus+1),*(pbDrvStatus+2),*(pbDrvStatus+3),*(pbMecSignal+0),*(pbMecSignal+1),*(pbMecSignal+2),*(pbMecSignal+3),
		*(pbUnvSignal+0),*(pbUnvSignal+1),*(pbUnvSignal+2),*(pbUnvSignal+3),*(pdwInternal+0),*(pdwInternal+1),*(pdwInternal+2),*(pdwInternal+3),
		*(pdwExternal+0),*(pdwExternal+1),*(pdwExternal+2),*(pdwExternal+3));
#endif

	DebugPrint(buffx);
#endif
	CTDwUnLock();	//2014.12.17 完了後、別のスレッドが入れるようにする
	return Result;
}











//-------------------------------------------------------------------
// 説  明		DLLエクスポート関数アドレスの取得
// 引  数		なし
// 戻り値		TRUE	成功
//				FALSE	失敗
//-------------------------------------------------------------------

static BOOL CTDwGetDllExportFunc( VOID )
{
	pCTDGetLibVersion	= (BOOL (WINAPI *)(PBYTE)) GetProcAddress( hDLL, "CTDGetLibVersion");
	if ( pCTDGetLibVersion == NULL ) return FALSE;
	pCTDGetDrvVersion	= (BOOL (WINAPI *)(PBYTE)) GetProcAddress( hDLL, "CTDGetDrvVersion");
	if ( pCTDGetDrvVersion == NULL ) return FALSE;

	pCTDCreate			= (int (WINAPI *)(WORD)) GetProcAddress( hDLL, "CTDCreate");
	if ( pCTDCreate == NULL ) return FALSE;
	pCTDClose			= (BOOL (WINAPI *)(WORD)) GetProcAddress( hDLL, "CTDClose");
	if ( pCTDClose == NULL ) return FALSE;

	pCTDGetRomVersion	= (BOOL (WINAPI *)(WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetRomVersion");
	if ( pCTDGetRomVersion == NULL ) return FALSE;

	pCTDInPort			= (BOOL (WINAPI *)(WORD,WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDInPort");
	if ( pCTDInPort == NULL ) return FALSE;
	pCTDOutPort			= (BOOL (WINAPI *)(WORD,WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDOutPort");
	if ( pCTDOutPort == NULL ) return FALSE;
	pCTDGetDriveStatus	= (BOOL (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetDriveStatus");
	if ( pCTDGetDriveStatus == NULL ) return FALSE;
	pCTDGetEndStatus	= (BOOL	(WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetEndStatus");
	if ( pCTDGetEndStatus == NULL ) return FALSE;
	pCTDGetMechanicalSignal =
		(BOOL (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetMechanicalSignal");
	if ( pCTDGetMechanicalSignal == NULL ) return FALSE;
	pCTDGetUniversalSignal =
		(BOOL (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetUniversalSignal");
	if ( pCTDGetUniversalSignal == NULL ) return FALSE;
	pCTDMode1Write		= (BOOL (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDMode1Write");
	if ( pCTDMode1Write == NULL ) return FALSE;
	pCTDMode2Write		= (BOOL (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDMode2Write");
	if ( pCTDMode2Write == NULL ) return FALSE;
	pCTDUniversalSignalWrite =
                                  (BOOL (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDUniversalSignalWrite");
	if ( pCTDUniversalSignalWrite == NULL ) return FALSE;

	pCTDDataHalfRead	= (BOOL (WINAPI *)(WORD,WORD,BYTE,PWORD)) GetProcAddress( hDLL, "CTDDataHalfRead");
	if ( pCTDDataHalfRead == NULL ) return FALSE;
	pCTDDataFullRead	= (BOOL (WINAPI *)(WORD,WORD,BYTE,PDWORD)) GetProcAddress( hDLL, "CTDDataFullRead");
	if ( pCTDDataFullRead == NULL ) return FALSE;

	pCTDCommandWrite	= (BOOL (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDCommandWrite");
	if ( pCTDCommandWrite == NULL ) return FALSE;

	pCTDDataHalfWrite	= (BOOL (WINAPI *)(WORD,WORD,BYTE,WORD)) GetProcAddress( hDLL, "CTDDataHalfWrite");
	if ( pCTDDataHalfWrite == NULL ) return FALSE;
	pCTDDataFullWrite	= (BOOL (WINAPI *)(WORD,WORD,BYTE,DWORD)) GetProcAddress( hDLL, "CTDDataFullWrite");
	if ( pCTDDataFullWrite == NULL ) return FALSE;

	pCTDCallBack  = (int (WINAPI *)(HANDLE,void (WINAPI *)(void)))GetProcAddress( hDLL, "CTDCallBack");
 	if ( pCTDCallBack == NULL ) return FALSE;

	pCTDFreeCallBack = (int (WINAPI *)(void)) GetProcAddress( hDLL,"CTDFreeCallBack");
	if ( pCTDFreeCallBack == NULL ) return FALSE;

	pCTDGetIoRead	= (BOOL (WINAPI *)(WORD,PDWORD)) GetProcAddress( hDLL, "CTDGetIoRead");
	if ( pCTDGetIoRead == NULL ) return FALSE;
	pCTDSetIoWrite	= (BOOL (WINAPI *)(WORD,WORD,PDWORD)) GetProcAddress( hDLL, "CTDSetIoWrite");
	if ( pCTDSetIoWrite == NULL ) return FALSE;

	pCTDGetLastError	= (DWORD (WINAPI *)(WORD)) GetProcAddress( hDLL, "CTDGetLastError");
	if ( pCTDGetLastError == NULL ) return FALSE;
	pCTDLineFallOut	= (BOOL (WINAPI *)(WORD,WORD,WORD)) GetProcAddress( hDLL, "CTDLineFallOut");
	if ( pCTDLineFallOut == NULL ) return FALSE;


	pCTDPDataWrite	= (BOOL (WINAPI *)(WORD,WORD,BYTE,DWORD,DWORD)) GetProcAddress( hDLL, "CTDPDataWrite");
	if ( pCTDPDataWrite == NULL ) return FALSE;
	pCTDPDataRead	= (BOOL (WINAPI *)(WORD,WORD,BYTE,DWORD,DWORD)) GetProcAddress( hDLL, "CTDPDataRead");
	if ( pCTDPDataRead == NULL ) return FALSE;
	pCTDPDriveRead	= (BOOL (WINAPI *)(WORD,WORD,BYTE,DWORD)) GetProcAddress( hDLL, "CTDPDriveRead");
	if ( pCTDPDriveRead == NULL ) return FALSE;
	pCTDPOutPort	= (BOOL (WINAPI *)(WORD,WORD,BYTE,BYTE,DWORD)) GetProcAddress( hDLL, "CTDPOutPort");
	if ( pCTDPOutPort == NULL ) return FALSE;
	pCTDPInPort	= (BOOL (WINAPI *)(WORD,WORD,BYTE,BYTE,BYTE,BYTE,DWORD,DWORD)) GetProcAddress( hDLL, "CTDPInPort");
	if ( pCTDPInPort == NULL ) return FALSE;
	pCTDPGStartStop	= (BOOL (WINAPI *)(WORD,BYTE)) GetProcAddress( hDLL, "CTDPGStartStop");
	if ( pCTDPGStartStop == NULL ) return FALSE;
	pCTDPGAllStep	= (BOOL (WINAPI *)(WORD,DWORD)) GetProcAddress( hDLL, "CTDPGAllStep");
	if ( pCTDPGAllStep == NULL ) return FALSE;
	pCTDPGWait	= (BOOL (WINAPI *)(WORD,DWORD,DWORD)) GetProcAddress( hDLL, "CTDPGWait");
	if ( pCTDPGWait == NULL ) return FALSE;
	pCTDPGEnd	= (BOOL (WINAPI *)(WORD,DWORD)) GetProcAddress( hDLL, "CTDPGEnd");
	if ( pCTDPGEnd == NULL ) return FALSE;
	pCTDPGStatus	= (BOOL (WINAPI *)(WORD,PDWORD,PBYTE)) GetProcAddress( hDLL, "CTDPGStatus");
	if ( pCTDPGStatus == NULL ) return FALSE;

	pCTDStartSignalWrite	= (BOOL (WINAPI *)(WORD,WORD)) GetProcAddress( hDLL, "CTDStartSignalWrite");
	if ( pCTDStartSignalWrite == NULL ) return FALSE;
	pCTDGetAxisStatus	= (BOOL (WINAPI *)(WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetAxisStatus");
	if ( pCTDGetAxisStatus == NULL ) return FALSE;
	pCTDGetAxisAllPort	= (BOOL (WINAPI *)(WORD,WORD,PDWORD)) GetProcAddress( hDLL, "CTDGetAxisAllPort");
	if ( pCTDGetAxisAllPort == NULL ) return FALSE;
	pCTDDeviceStatus	= (BOOL (WINAPI *)(WORD,PBYTE)) GetProcAddress( hDLL, "CTDDeviceStatus");
	if ( pCTDDeviceStatus == NULL ) return FALSE;
	
	//API関数
	pCTDSetControl = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDSetControl");
	if ( pCTDSetControl == NULL ) return FALSE;
	pCTDGetControl = (int (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetControl");
	if ( pCTDGetControl == NULL ) return FALSE;
	
	pCTDSetSoftLimit = (int (WINAPI *)(WORD,WORD,DWORD,DWORD,WORD,WORD)) GetProcAddress( hDLL, "CTDSetSoftLimit");
	if ( pCTDSetSoftLimit == NULL ) return FALSE;
	pCTDGetSoftLimit = (int (WINAPI *)(WORD,WORD,PDWORD,PDWORD,PWORD,PWORD)) GetProcAddress( hDLL, "CTDGetSoftLimit");
	if ( pCTDGetSoftLimit == NULL ) return FALSE;
	
	pCTDGetVersion = (int (WINAPI *)(PBYTE,PBYTE)) GetProcAddress( hDLL, "CTDGetVersion");
	if ( pCTDGetVersion == NULL ) return FALSE;
	pCTDGetBoardStatus = (int (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetBoardStatus");
	if ( pCTDGetBoardStatus == NULL ) return FALSE;
	pCTDGetCounter = (int (WINAPI *)(WORD,WORD,BYTE,PDWORD)) GetProcAddress( hDLL, "CTDGetCounter");
	if ( pCTDGetCounter == NULL ) return FALSE;
	
	pCTDSetSpeedParameter = (int (WINAPI *)(WORD,WORD,PCTDSPDPARAMETER)) GetProcAddress( hDLL, "CTDSetSpeedParameter");
	if ( pCTDSetSpeedParameter == NULL ) return FALSE;
	pCTDGetSpeedParameter = (int (WINAPI *)(WORD,WORD,PCTDSPDPARAMETER)) GetProcAddress( hDLL, "CTDGetSpeedParameter");
	if ( pCTDGetSpeedParameter == NULL ) return FALSE;
	pCTDSpeedOverride = (int (WINAPI *)(WORD,WORD,DWORD)) GetProcAddress( hDLL, "CTDSpeedOverride");
	if ( pCTDSpeedOverride == NULL ) return FALSE;
	pCTDGetNowSpeed = (int (WINAPI *)(WORD,WORD,PDWORD)) GetProcAddress( hDLL, "CTDGetNowSpeed");
	if ( pCTDGetNowSpeed == NULL ) return FALSE;
	
	pCTDDriveStart = (int (WINAPI *)(WORD,WORD,BYTE,DWORD)) GetProcAddress( hDLL, "CTDDriveStart");
	if ( pCTDDriveStart == NULL ) return FALSE;
	pCTDDriveStop = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDDriveStop");
	if ( pCTDDriveStop == NULL ) return FALSE;
	pCTDPulseOverride = (int (WINAPI *)(WORD,WORD,DWORD)) GetProcAddress( hDLL, "CTDPulseOverride");
	if ( pCTDPulseOverride == NULL ) return FALSE;
	pCTDSyncONOFF = (int (WINAPI *)(WORD,BYTE)) GetProcAddress( hDLL, "CTDSyncONOFF");
	if ( pCTDSyncONOFF == NULL ) return FALSE;
	
	pCTDORGSignalStart = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDORGSignalStart");
	if ( pCTDORGSignalStart == NULL ) return FALSE;
	pCTDORGSignalStop = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDORGSignalStop");
	if ( pCTDORGSignalStop == NULL ) return FALSE;
	pCTDORGGetStatus = (int (WINAPI *)(WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDORGGetStatus");
	if ( pCTDORGGetStatus == NULL ) return FALSE;
	pCTDORGSetPosiData = (int (WINAPI *)(WORD,WORD,DWORD)) GetProcAddress( hDLL, "CTDORGSetPosiData");
	if ( pCTDORGSetPosiData == NULL ) return FALSE;
	
	pCTDSetDriveONOFF = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDSetDriveONOFF");
	if ( pCTDSetDriveONOFF == NULL ) return FALSE;
	pCTDSetDrstONOFF = (int (WINAPI *)(WORD,WORD,BYTE)) GetProcAddress( hDLL, "CTDSetDrstONOFF");
	if ( pCTDSetDrstONOFF == NULL ) return FALSE;
	
	pCTDGetALMCode = (int (WINAPI *)(WORD,WORD,WORD,PBYTE)) GetProcAddress( hDLL, "CTDGetALMCode");
	if ( pCTDGetALMCode == NULL ) return FALSE;
	pCTDALMCLR = (int (WINAPI *)(WORD,WORD,WORD)) GetProcAddress( hDLL, "CTDALMCLR");
	if ( pCTDALMCLR == NULL ) return FALSE;
	pCTDParaCont = (int (WINAPI *)(WORD,WORD,BYTE,WORD,WORD,DWORD,PDWORD)) GetProcAddress( hDLL, "CTDParaCont");
	if ( pCTDParaCont == NULL ) return FALSE;
	pCTDEvent = (int (WINAPI *)(WORD,WORD,BYTE,WORD,WORD,PDWORD)) GetProcAddress( hDLL, "CTDEvent");
	if ( pCTDEvent == NULL ) return FALSE;

	//14.12.17
	pCTLock = (void (WINAPI *)(void)) GetProcAddress( hDLL, "CTLock");
	if ( pCTLock == NULL ) return FALSE;
	pCTUnLock = (void (WINAPI *)(void)) GetProcAddress( hDLL, "CTUnLock");
	if ( pCTUnLock == NULL ) return FALSE;
	pCTDGetStatusCounterALL = (int (WINAPI *)(WORD,PBYTE,PBYTE,PBYTE,PDWORD,PDWORD)) GetProcAddress( hDLL, "CTDGetStatusCounterALL");
	if ( pCTDGetStatusCounterALL == NULL ) return FALSE;

	return TRUE;	// 全てのアドレス取得完了
}

