/*****************************************************************************
 * ctdw.h
 *
 * PCPG168NI制御
 * Copyright CocmoTechs Co.,Ltd.
 *
 * Date 4.2005
 *****************************************************************************/
#pragma once
#include "CTD.h"

#define CTD_DLL_NAME		"CTD20.dll"			// DLL 名
#define CTD_ERR_WRAPDLL		((DWORD)-1)		    // ラッパー関数内エラー

extern HINSTANCE hDLL;

//USPG28 USPG48 USPG48H用関数定義
BOOL	APIENTRY CTDwDllOpen( void );
BOOL	APIENTRY CTDwDllClose( void );
BOOL	APIENTRY CTDwGetLibVersion( PBYTE pbLibVer);
BOOL	APIENTRY CTDwGetDrvVersion( PBYTE pbDrvVer);
BOOL	APIENTRY CTDwGetRomVersion(WORD wBsn, PBYTE pbRomVer);
DWORD	APIENTRY CTDwGetLastError(WORD wBsn);
BOOL	APIENTRY CTDwCreate( WORD wBsn );
BOOL	APIENTRY CTDwClose( WORD wBsn );
BOOL	APIENTRY CTDwLineFallOut(WORD wBsn, WORD wAxis, WORD wData);
BOOL	APIENTRY CTDwGetIoRead( WORD wBsn,PDWORD pdwData );
BOOL	APIENTRY CTDwGetIoReadByte( WORD wBsn,BYTE badr,PBYTE pbData );
BOOL	APIENTRY CTDwSetIoWrite( WORD wBsn,WORD wData ,PDWORD pdwData );
BOOL	APIENTRY CTDwSetIoWriteByte(WORD wBsn,BYTE badr,BYTE bData);
BOOL	APIENTRY CTDwInPort( WORD wBsn, WORD wAxis ,WORD wPort, PBYTE pbData );
BOOL	APIENTRY CTDwOutPort( WORD wBsn, WORD wAxis ,WORD wPort, BYTE bData );
BOOL	APIENTRY CTDwGetDriveStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus);
BOOL	APIENTRY CTDwGetEndStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus);
BOOL	APIENTRY CTDwGetMechanicalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal );
BOOL	APIENTRY CTDwGetUniversalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal );
BOOL	APIENTRY CTDwMode1Write( WORD wBsn, WORD wAxis, BYTE bMode );
BOOL	APIENTRY CTDwMode2Write( WORD wBsn, WORD wAxis, BYTE bMode );
BOOL	APIENTRY CTDwUniversalSignalWrite( WORD wBsn, WORD wAxis, BYTE bSignal );
BOOL	APIENTRY CTDwDataRead( WORD wBsn, WORD wAxis, BYTE bCmd, PBYTE pbData );
BOOL	APIENTRY CTDwDataHalfRead( WORD wBsn, WORD wAxis, BYTE bCmd, PWORD pwData );
BOOL	APIENTRY CTDwDataFullRead( WORD wBsn, WORD wAxis, BYTE bCmd, PDWORD pdwData );
BOOL	APIENTRY CTDwCommandWrite( WORD wBsn, WORD wAxis, BYTE bCmd );
BOOL	APIENTRY CTDwDataWrite( WORD wBsn, WORD wAxis, BYTE bCmd, BYTE bData );
BOOL	APIENTRY CTDwDataHalfWrite( WORD wBsn, WORD wAxis, BYTE bCmd, WORD wData );
BOOL	APIENTRY CTDwDataFullWrite( WORD wBsn, WORD wAxis, BYTE bCmd, DWORD dwData );

BOOL	APIENTRY CTDwSpeedParameterWrite(WORD wBsn, WORD wAxis, double dLowSpeed, double dHighSpeed,short sAccTime, double dSRate);
BOOL	APIENTRY CTDwSpeedWrite(WORD wBsn, WORD wAxis, double dObjSpeed);
BOOL	APIENTRY CTDwSpeedParameterRead(WORD wBsn, WORD wAxis, double *pdLowSpeed, double *pdHighSpeed,short *psAccTime, double *pdSRate);
BOOL	APIENTRY CTDwSpeedRead(WORD wBsn, WORD wAxis, double *pdObjSpeed);

BOOL	APIENTRY CTDwGetInternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData);
BOOL	APIENTRY CTDwGetExternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData);
BOOL	APIENTRY CTDwGetDrivePulseCounter(WORD wBsn, WORD wAxis, PDWORD pdwData);
BOOL	APIENTRY CTDwGetNowSpeedData(WORD wBsn, WORD wAxis, PWORD pwData);

BOOL	APIENTRY CTDwPOutPort(WORD wBsn,WORD wAxis,BYTE bAdr,BYTE bdata, DWORD dwStepNo );
BOOL	APIENTRY CTDwPDataWrite(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwData, DWORD dwStepNo );
BOOL	APIENTRY CTDwPDataRead(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwANDData, DWORD dwStepNo );
BOOL	APIENTRY CTDwPInPort(WORD wBsn,WORD wAxis,BYTE bIoAdr,BYTE bCmpData,BYTE bActiveLevel,BYTE bCondition,DWORD dwJmpStepNo, DWORD dwStepNo );
BOOL	APIENTRY CTDwPDriveRead(WORD wBsn,WORD wAxis,BYTE bSetData, DWORD dwStepNo );
BOOL	APIENTRY CTDwPGStartStop(WORD wBsn,BYTE bSS );
BOOL	APIENTRY CTDwPGAllStep(WORD wBsn,DWORD dwStepNo );
BOOL	APIENTRY CTDwPGWait(WORD wBsn,DWORD dwWaitTime,DWORD dwStepNo );
BOOL	APIENTRY CTDwPGEnd(WORD wBsn,DWORD dwStepNo);
BOOL	APIENTRY CTDwPGStatus(WORD wBsn,PDWORD pdwStepNo, PBYTE pbStatePg);

int		APIENTRY CTDwCallBack(HANDLE hMutex,void (WINAPI *lpfnHandler)(void));
int		APIENTRY CTDwFreeCallBack(void);

BOOL	APIENTRY CTDwStartSignalWrite(WORD wBsn,WORD wStart);
BOOL	APIENTRY CTDwGetAxisStatus( WORD wBsn, PBYTE pbData );
BOOL	APIENTRY CTDwGetAxisAllPort( WORD wBsn, WORD wAxis, PDWORD pdwData );
BOOL	APIENTRY CTDwDeviceStatus( WORD wBsn, PBYTE pbStatus);

//API関数
int		APIENTRY CTDwSetControl( WORD wBsn, WORD wAxis, BYTE bControl );
int		APIENTRY CTDwGetControl( WORD wBsn, WORD wAxis, PBYTE pbControl );

int 	APIENTRY CTDwSetSoftLimit( WORD wBsn, WORD wAxis, DWORD dwHighLimit, DWORD dwLowLimit, WORD wMode, WORD wEvent );
int 	APIENTRY CTDwGetSoftLimit( WORD wBsn, WORD wAxis, PDWORD pdwHighLimit, PDWORD pdwLowLimit, PWORD pwMode, PWORD pwEvent );

int 	APIENTRY CTDwGetVersion( PBYTE pbDLLVersion, PBYTE pbDrvVersion ) ;
int 	APIENTRY CTDwGetBoardStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus );
int 	APIENTRY CTDwGetCounter( WORD wBsn, WORD wAxis, BYTE bMode, PDWORD pdwData );

int		APIENTRY CTDwSetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData ) ;
int		APIENTRY CTDwGetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData );
int		APIENTRY CTDwSpeedOverride( WORD wBsn, WORD wAxis, DWORD dwHighSpeed );
int		APIENTRY CTDwGetNowSpeed( WORD wBsn, WORD wAxis, PDWORD pdwNowSpeed );

int		APIENTRY CTDwDriveStart( WORD wBsn, WORD wAxis, BYTE bMode, DWORD dwPulse );
int		APIENTRY CTDwDriveStop( WORD wBsn, WORD wAxis, BYTE bStop );
int		APIENTRY CTDwPulseOverride( WORD wBsn, WORD wAxis, DWORD dwPulse );
int		APIENTRY CTDwSyncONOFF( WORD wBsn, BYTE bONOFF );

int		APIENTRY CTDwORGSignalStart( WORD wBsn, WORD wAxis, BYTE bOrgMode );
int		APIENTRY CTDwORGSignalStop( WORD wBsn, WORD wAxis, BYTE bStop );
int		APIENTRY CTDwORGGetStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus );
int		APIENTRY CTDwORGSetPosiData( WORD wBsn, WORD wAxis, DWORD dwPosi );

int		APIENTRY CTDwSetDriveONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF );
int		APIENTRY CTDwSetDrstONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF );

int		APIENTRY CTDwGetALMCode( WORD wBsn, WORD wAxis, WORD wValue, PBYTE pbCode ) ;
int		APIENTRY CTDwALMCLR( WORD wBsn, WORD wAxis, WORD wValue );
int		APIENTRY CTDwParaCont( WORD wBsn, WORD wAxis, BYTE bType, WORD wClassNo, WORD wParaNo, DWORD dwData, PDWORD pdwValue );
int		APIENTRY CTDwEvent( WORD wBsn, WORD wAxis, BYTE bKind, WORD wCmd, WORD wValue, PDWORD pdwValue );

//14.12.17
BOOL	APIENTRY CTDwLock( void );
BOOL	APIENTRY CTDwUnLock( void );
int		APIENTRY CTDwGetStatusCounterALL( WORD wBsn, PBYTE pbDrvStatus, PBYTE pbMecSignal, PBYTE pbUnvSignal, PDWORD pdwInternal, PDWORD pdwExternal );