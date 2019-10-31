//===================================================================
// CTDw.c
//-------------------------------------------------------------------
// CTD����
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

// -- �X�^�e�B�b�N�֐� & �X�^�e�B�b�N�ϐ��i�萔�j
static BOOL CTDwGetDllExportFunc( VOID );

//// -- �G�N�X�|�[�g�֐��A�h���X�̊i�[�p�ϐ�
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
// ��  ��		DLL ���A�����[�h
// ��  ��		�Ȃ�
// �߂�l		TRUE	����
//				FALSE	���s
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
// ��  ��		DLL �����[�h
// ��  ��		�Ȃ�
// �߂�l		TRUE	����
//				FALSE	���s
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
// ��  ��		�o�[�W�����ԍ��擾
// ��  ��		pbLibVer	DLL �o�[�W�������i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
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
// ��  ��		�o�[�W�����ԍ��擾
// ��  ��		pbDrvVer	DRV �o�[�W�������i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
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
// ��  ��		�f�o�C�X�̎g�p��錾
// ��  ��		wBsn	�a�r�m�i�{�[�h�Z���N�g�i���o�[�j
//						
// �߂�l		TRUE	����
//				-1		���s
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
// ��  ��		�f�o�C�X�̉��
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
// �߂�l		TRUE	����
//				FALSE	���s
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
// ��  ��		�o�[�W�����ԍ��擾
// ��  ��		pbRomVer	ROM �o�[�W�������i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwGetRomVersion(WORD wBsn, PBYTE pbRomVer)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�f�o�C�X�̏�Ԃ��擾
// ��  ��		wBsn			�{�[�h�Z���N�g�i���o�[
//				pdStatus		�f�o�C�X�̏�Ԃ��i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwDeviceStatus( WORD wBsn, PBYTE pbStatus)
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	return (*pCTDDeviceStatus)(wBsn,pbStatus);
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
}
//===================================================================
// ��  ��		�G���[�R�[�h�擾
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
// �߂�l		�G���[�R�[�h
//===================================================================
DWORD APIENTRY CTDwGetLastError(WORD wBsn)
{
DWORD iError;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return iError;
}
//===================================================================
// ��  ��		�R�l�N�^���������̏o�͓���ݒ�
// ��  ��		wBsn	BSN
//				wData	����p�����[�^
//						 �O�F������~���ߏo��
//						 �P�F����~���ߏo��
//						 �Q�F�����o�͂��Ȃ�
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwLineFallOut(WORD wBsn, WORD wAxis, WORD wData)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�ǂݍ��ݎ��s
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				wPort	�Ώۃ|�[�g�ԍ�
//				pbData	�ǂݍ��񂾃f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwInPort( WORD wBsn, WORD wAxis ,WORD wPort, PBYTE pbData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�������ݎ��s
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				wPort	�Ώۃ|�[�g�ԍ�
//				bData	�������ޒl
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwOutPort( WORD wBsn, WORD wAxis ,WORD wPort, BYTE bData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�h���C�u�X�e�[�^�X�擾
// ��  ��		wBsn		�{�[�h�Z���N�g�i���o�[
//				wAxis		�Ώې��䎲
//				pbStatus	�����i�[����̈�ւ̃|�C���^	
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetDriveStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�G���h�X�e�[�^�X�擾
// ��  ��		wBsn		�{�[�h�Z���N�g�i���o�[
//				wAxis		�Ώې��䎲
//				pbStatus	�����i�[����̈�ւ̃|�C���^	
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetEndStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus)
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		���J�j�J���V�O�i���擾
// ��  ��		wBsn		�{�[�h�Z���N�g�i���o�[
//				wAxis		�Ώې��䎲
//				pbSignal	�����i�[����̈�ւ̃|�C���^	
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetMechanicalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		���j�o�[�T���V�O�i���擾
// ��  ��		wBsn		�{�[�h�Z���N�g�i���o�[
//				wAxis		�Ώې��䎲
//				pbSignal	�����i�[����̈�ւ̃|�C���^	
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetUniversalSignal( WORD wBsn, WORD wAxis, PBYTE pbSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		���[�h�P��������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bMode	�������ރf�[�^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwMode1Write( WORD wBsn, WORD wAxis, BYTE bMode )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		���[�h�Q��������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bMode	�������ރf�[�^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwMode2Write( WORD wBsn, WORD wAxis, BYTE bMode )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		���j�o�[�T���V�O�i����������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bSignal	�������ރf�[�^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL  APIENTRY CTDwUniversalSignalWrite( WORD wBsn, WORD wAxis, BYTE bSignal )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�Q�o�C�g�f�[�^�ǂݍ���
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bCmd	�ǂݍ��ݎ��Ɏ��s����R�}���h
//				pwData	�ǂݍ��񂾃f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY  CTDwDataHalfRead( WORD wBsn, WORD wAxis, BYTE bCmd, PWORD pwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
BOOL  APIENTRY CTDwDataRead( WORD wBsn, WORD wAxis, BYTE bCmd, PBYTE pbData )
{
BOOL bln;
WORD wData;

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�S�o�C�g�f�[�^�ǂݍ���
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bCmd	�ǂݍ��ݎ��Ɏ��s����R�}���h
//				pdwData	�ǂݍ��񂾃f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwDataFullRead( WORD wBsn, WORD wAxis, BYTE bCmd, PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�R�}���h��������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bCmd	�R�}���h
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwCommandWrite( WORD wBsn, WORD wAxis, BYTE bCmd )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�Q�o�C�g�f�[�^��������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bCmd	�������ݎ��Ɏ��s����R�}���h
//				wData	�������ރf�[�^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwDataHalfWrite( WORD wBsn, WORD wAxis, BYTE bCmd, WORD wData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
BOOL APIENTRY CTDwDataWrite( WORD wBsn, WORD wAxis, BYTE bCmd, BYTE bData )
{
BOOL bln;
WORD wData;

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		�S�o�C�g�f�[�^��������
// ��  ��		wBsn	�{�[�h�Z���N�g�i���o�[
//				wAxis	�Ώې��䎲
//				bCmd	�������ݎ��Ɏ��s����R�}���h
//				dwData	�������ރf�[�^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwDataFullWrite( WORD wBsn, WORD wAxis, BYTE bCmd, DWORD dwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
// ��  ��		IO�ǂݏo��
// ��  ��		wBsn		BSN
//			    pdwData     32bit Data Read
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetIoRead( WORD wBsn,PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
//badr =0:0byte 1:1byte 2:2byte 3:3byte

BOOL APIENTRY CTDwGetIoReadByte( WORD wBsn,BYTE badr,PBYTE pbData )
{
DWORD dwData;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}
//===================================================================
// ��  ��		IO��������
// ��  ��		wBsn		BSN
//			    wData       16bit Data Write
//              pdwData     �������ݎ��s���AFF000000h ��Ԃ��܂��B
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwSetIoWrite( WORD wBsn,WORD wData ,PDWORD pdwData )
{
BOOL bln;
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return bln;
}
//===================================================================
//badr =0:0byte 1:1byte
BOOL APIENTRY CTDwSetIoWriteByte(WORD wBsn,BYTE badr,BYTE bData)
{
DWORD dwData;
WORD  w;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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


	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}



//---------------------------------------------------------------------
//				���x�֘A�@�\
//---------------------------------------------------------------------

//====================================================================
//		    ����	���x�p�����[�^�[
//			�����F	wBsn	=BSN�ԍ�
//					wAxis	=���䎲
//					wLospd	=START/STOP SPEED
//					wHiSpd	=OBJECT SPEED
//					wRange	=Range Data
//					wRate	=RATE1
//					wSrate  =SW1
//					STorSW  =�������[�h�@or S�����[�h
//
//			�߂�l�FTRUE	=����
//					FALSE	=���s
//====================================================================

BOOL SetSpd(WORD wBsn, WORD wAxis, WORD wLoSpd, WORD wHiSpd, WORD wRange, WORD wRate,
				   WORD wSRate, short STorSW)
{
	/* �ݒ� */
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
//			�����@�@���x�p�����[�^�[�ݒ�
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//					dLowSpeed	=�J�n���x(PPS)
//					dHighSpeed  =�ō����x(PPS)
//					sAccTime	=����������(msec)
//					dSRate		=S��������(%)
//
//			�߂�l: TRUE		=����
//					FALSE		=���s
//					CAUTION
//====================================================================

BOOL APIENTRY CTDwSpeedParameterWrite(WORD wBsn, WORD wAxis, double dLowSpeed, double dHighSpeed,
									  short sAccTime, double dSRate)
{
	double	dBuf, dMaxSpd, dRng, dHiSpd, dLoSpd, dSpd, dRate, dSw, tim, sr, st, dAcc, dObjSpeed;
	short	sRet, sBuf, sCtrl = 0;
	long	lngTemp;


	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	sr = 0.0;
	st = 0.0;

	sRet = TRUE;


	if (dSRate == CTD_STRAIGHT_MOVE)
		sBuf = CTD_STRAIGHT_MOVE;
	else
		sBuf = 0;


	/* �J�n���x��0�� */
	if (dLowSpeed == 0.0){
		/* �ō����x���~�b�g�`�F�b�N */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
		/* �ō����x�����������͂ł��Ă��� */
		/* �������x���~�b�g�`�F�b�N */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
		/* ���������Ԃ�0�̏ꍇ */
		if (sAccTime == 0){
			sCtrl = 1;				/* �����`���� */
		/* ���������Ԃ����������͂ł��Ă����ꍇ */
		}else{
			sCtrl = 2;				/* ���N�����x�������I��1�ɂ�����`���� */
		}
	/* �J�n���x��0�ȏ�� */
	}else if (dLowSpeed > 0.0){
		/* �ō����x���~�b�g�`�F�b�N */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
		/* �ō����x�����������͂ł��Ă��� */
		/* �J�n���x�ƍō����x�������ꍇ */
		if (dLowSpeed == dHighSpeed){
			sCtrl = 1;				/* �����`���� */
		/* �J�n���x�Ɖ������x���Ⴄ�ꍇ */
		}else{
			sCtrl = 5;				/* �ʏ퐧�� */
		}

	}

	/* ���������Ԃ�0�� */
	if (sAccTime == 0){
		/* �ō����x�����������͂ł��Ă��� */
		/* �������x���~�b�g�`�F�b�N */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
		/* �J�n���x���ō����x���傫�Ȓl�̏ꍇ */
		if (dLowSpeed <= dHighSpeed){
			sCtrl = 1;				/* �����`���� */
		/* ���������͂ł��Ă��Ȃ�������G���[ */
		}else{
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
	}
	//USPG28 or USPG48
	dObjSpeed = SpdParam[wBsn].dObjSpd[wAxis];

	/* �����`���� */
	if (sCtrl == 1){
		/* �ō����x���~�b�g�`�F�b�N */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �����W�v�Z */
		dRng = 8120000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* �����W���~�b�g�`�F�b�N */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* �Z�o���ꂽ�����W�ōō��̎��g�������߂� */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW���x���~�b�g�`�F�b�N */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* ���N�����x�ɍō����x�̒l���� */
		dLoSpd = dHiSpd;

		/* ���[�g�AS���[�g�������I��1�������B�Ӗ������p�����[�^ */
		dRate = 1.0;
		dSw = 1.0;

		/* ���x�ݒ� */
		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
						   (WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return TRUE;
	/* ��`����F�����I�Ɏ��N������ */
	}else if (sCtrl == 2){
		/* �����I�Ɏ��N����1�ɂ��� */
		dLoSpd = 1;

		/* �ō����x���~�b�g�`�F�b�N */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8190000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �������x���~�b�g�`�F�b�N */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* ���������~�b�g�`�F�b�N */
		if ((dSRate < -1.0) || (dSRate > 100.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �����W�v�Z */
		dRng = 8190000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* �����W���~�b�g�`�F�b�N */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* �Z�o���ꂽ�����W�ōō��̎��g�������߂� */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW���x���~�b�g�`�F�b�N */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* �ō����x�ƊJ�n���x�̍��Z�o */
		dSpd = dHiSpd - dLoSpd;

		/* ��Βl�ɕϊ� */
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

		/* ���[�g���~�b�g�`�F�b�N */
		if (dRate < 1.0){
			dRate = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dRate > 8190.0){
			dRate = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* S����ԑ��x�f�[�^�v�Z */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* S����ԃ��~�b�g�`�F�b�N */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (dLoSpd > dHiSpd)
			dLoSpd = dHiSpd;

		/* �ݒ� */
		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
						   (WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return sRet;	//2015.09.16 return TRUE��sRet�ɏC�� beppu
	}else if(sCtrl == 5){
		/* �J�n���x�Œ჊�~�b�g�`�F�b�N */
		if ((dLowSpeed <= 0.0) || (dLowSpeed > 4095000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �ō����x���~�b�g�`�F�b�N */
		if ((dHighSpeed <= 0.0) || (dHighSpeed > 8191000.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �J�n���x���ō����x�����傫�Ȓl�������� */
		if (dHighSpeed < dLowSpeed){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �������x���~�b�g�`�F�b�N */
		if ((sAccTime < 0) || (sAccTime > 9999)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* ���������~�b�g�`�F�b�N */
		if ((dSRate < -1.0) || (dSRate > 100.0)){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �����W�v�Z */
		dRng = 8190000.0 / dHighSpeed;

		lngTemp = (long)dRng;
		dRng = (double)lngTemp;

		/* �����W���~�b�g�`�F�b�N */
		if (dRng < 1.0) dRng = 1.0;
		if (dRng > 8190.0) dRng = 8190.0;

		/* �Z�o���ꂽ�����W�ōō��̎��g�������߂� */
		dMaxSpd = dHighSpeed * (100.0 / 100.0);
		dMaxSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW */
		dHiSpd = dHighSpeed * (dObjSpeed / 100.0);
		dHiSpd *= (dRng / (double)BAS_RNG);

		/* �ڕW���x���~�b�g�`�F�b�N */
		if (dHiSpd < 1.0) dHiSpd = 1.0;
		if (dHiSpd > 8190.0) dHiSpd = 8190.0;

		/* �J�n���x�f�[�^�Z�o */
		dLoSpd = dLowSpeed * (dRng / (double)BAS_RNG);

		/* �J�n���x���~�b�g�`�F�b�N */
		if (dLoSpd < 1.0){
			dLoSpd = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dLoSpd > 8190.0){
			dLoSpd = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* �ō����x�ƊJ�n���x�̍��Z�o */
		dSpd = dHiSpd - dLoSpd;

		/* ��Βl�ɕϊ� */
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

		/* ���[�g���~�b�g�`�F�b�N */
		if (dRate < 1.0){
			dRate = 1.0;
			sRet = CTD_CAUTION;
		}
		if (dRate > 8190.0){
			dRate = 8190.0;
			sRet = CTD_CAUTION;
		}

		/* S����ԑ��x�f�[�^�v�Z */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* S����ԃ��~�b�g�`�F�b�N */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (dLoSpd > dHiSpd)
			dLoSpd = dHiSpd;

		if (FALSE == SetSpd(wBsn, wAxis, (WORD)(dLoSpd + 0.5), (WORD)(dHiSpd + 0.5), (WORD)(dRng + 0.5),
			(WORD)(dRate + 0.5), (WORD)(dSw + 0.5), sBuf)){

			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		//USPG28 or USPG48
		SpdParam[wBsn].dLoSpd[wAxis] = dLowSpeed;
		SpdParam[wBsn].dHiSpd[wAxis] = dHighSpeed;
		SpdParam[wBsn].sUpDnSpd[wAxis] = sAccTime;
		SpdParam[wBsn].dSCurve[wAxis] = dSRate;

		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return sRet;
	}
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return FALSE;
}

//===================================================================
//			�����@�@���x�p�����[�^�[�ݒ�
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//					dObjSpeed	=�ړ����x(%)
//
//			�߂�l: TRUE		=����
//					FALSE		=���s
//					CAUTION
//===================================================================

BOOL APIENTRY CTDwSpeedWrite(WORD wBsn, WORD wAxis, double dObjSpeed)
{
	WORD	wBuf;
	double	dHiSpd, dLoSpd, dObjSpd, dRng, dSw, tim, sr, st, dBuf, dAcc;
	short	sRet = TRUE;

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	/* ���~�b�g�`�F�b�N */
	if ((dObjSpeed <= 0.0) || (dObjSpeed > 100.0)){
		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return FALSE;
	}

	/* Object Speed���擾 */
	if (FALSE == CTDwDataHalfRead(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_READ, &wBuf)){
		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return FALSE;
	}
	dObjSpd = (double)wBuf;

	/* �J�n���x�f�[�^�Z�o */
	if (FALSE == CTDwDataHalfRead(wBsn, wAxis, CTD_RANGE_READ, &wBuf)){
		CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
		return FALSE;
	}
	dRng = (double)wBuf;

	/* �Z�o���ꂽ�����W�ōō��̎��g�������߂� */
	/* �ڕW */
	dHiSpd = SpdParam[wBsn].dHiSpd[wAxis] * (dObjSpeed / 100.0);	//USPG28 or USPG48

	dHiSpd *= (dRng / (double)BAS_RNG);

	/* �ڕW���x���~�b�g�`�F�b�N */
	if (dHiSpd < 1.0) dHiSpd = 1.0;
	if (dHiSpd > 8190.0) dHiSpd = 8190.0;

	dLoSpd = SpdParam[wBsn].dLoSpd[wAxis] * (dRng / (double)BAS_RNG);	//USPG28 or USPG48

	/* �J�n���x���~�b�g�`�F�b�N */
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
		/* �Œᑬ�x�ݒ� */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, (WORD)(dLoSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �ō����x�ݒ� */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, (WORD)(dHiSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
	}else if(dObjSpeed > SpdParam[wBsn].dObjSpd[wAxis]){
		/* �ō����x�ݒ� */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_OBJECT_SPEED_DATA_WRITE, (WORD)(dHiSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}

		/* �Œᑬ�x�ݒ� */
		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_START_STOP_SPEED_DATA_WRITE, (WORD)(dLoSpd + 0.5))){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
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

		/* S����ԑ��x�f�[�^�v�Z */
		dBuf = ((sr + st) * (double)BAS_RTF) / (dHiSpd - dLoSpd);
		dSw = sr * (double)BAS_RTF / 2.0 / dBuf;

		/* ���~�b�g�`�F�b�N */
		if (dSw < 1.0) dSw = 1.0;
		if (dSw > 4095.0) dSw = 4095.0;

		if (FALSE == CTDwDataHalfWrite(wBsn, wAxis, CTD_SW1_DATA_WRITE, (WORD)(dSw + 0.5))){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
		}
	}

	SpdParam[wBsn].dObjSpd[wAxis] = dObjSpeed;

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return sRet;
}

//====================================================================
//			�����@�@���x�p�����[�^�[�ݒ�
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			
//			�߂�l:
//					*pdLowSpeed	=�J�n���x(PPS)
//					*pdHighSpeed=�ō����x(PPS)
//					*psAccTime	=����������(msec)
//					*pdSRate	=S��������(%)
//					TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL  APIENTRY  CTDwSpeedParameterRead(WORD wBsn, WORD wAxis, double *pdLowSpeed, double *pdHighSpeed,
									 short *psAccTime, double *pdSRate)
{

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	*pdLowSpeed = SpdParam[wBsn].dLoSpd[wAxis];
	*pdHighSpeed = SpdParam[wBsn].dHiSpd[wAxis];
	*psAccTime = SpdParam[wBsn].sUpDnSpd[wAxis];
	*pdSRate = SpdParam[wBsn].dSCurve[wAxis];

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}

//====================================================================
//			�����@�@���x�p�����[�^�[�ݒ�
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			�߂�l:
//					*pdObjSpeed	=�ړ����x(%)
//				 	TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL APIENTRY CTDwSpeedRead(WORD wBsn, WORD wAxis, double *pdObjSpeed)
{

	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	*pdObjSpeed = SpdParam[wBsn].dObjSpd[wAxis];

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}


//====================================================================
//			�����@�@�C���^�[�i���J�E���^�[�ǂݍ���
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			�߂�l:
//					*pdwData	=�C���^�[�i���J�E���^�[
//				 	TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL APIENTRY CTDwGetInternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    if( CTDwDataFullRead(wBsn,wAxis,0x29,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}

//====================================================================
//			�����@�@�G�N�X�^�[�i���J�E���^�[�ǂݍ���
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			�߂�l:
//					*pdwData	=�G�N�X�^�[�i���J�E���^�[
//				 	TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL  APIENTRY CTDwGetExternalCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    if( CTDwDataFullRead(wBsn,wAxis,0x2d,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}
//====================================================================
//			�����@�@�h���C�u�p���X�J�E���^�[�ǂݍ���
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			�߂�l:
//					*pdwData	=�h���C�u�p���X�J�E���^�[
//				 	TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL APIENTRY CTDwGetDrivePulseCounter(WORD wBsn, WORD wAxis, PDWORD pdwData)
{

	if ( hDLL == NULL ) return FALSE;
 	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    if( CTDwDataFullRead(wBsn,wAxis,0x13,pdwData) == FALSE){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
	}
	
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}
//====================================================================
//			�����@�@NOW SPEED DATA �ǂݍ���
//			�����F	wBSN		=BSN�ԍ�
//					wAxis		=���䎲
//			�߂�l:
//					*pwData		=NOW SPEED DATA
//				 	TRUE		=����
//					FALSE		=���s
//====================================================================

BOOL APIENTRY CTDwGetNowSpeedData(WORD wBsn, WORD wAxis, PWORD pwData)
{

	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    if( CTDwDataHalfRead(wBsn,wAxis,0x12,pwData) == FALSE){
			CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
			return FALSE;
	}

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}


//===================================================================
// ��  ��		CALL BACK �֐�
// ��  ��		HANDLE	:hMutex
//				void (WINAPI *lpfnHandler)(void)
//						:CALL BACK�֐��A�h���X
// �߂�l		0=	����
//			   -1=	���s
//===================================================================
int APIENTRY CTDwCallBack(HANDLE hMutex,void (WINAPI *lpfnHandler)(void))
{
int bln;

    if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    bln = (*pCTDCallBack)(hMutex,lpfnHandler);

#if _DEBUG 
	DebugPrint("CallBack \n");
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
    return bln;
}
//===================================================================
// ��  ��		CALL BACK �J��
// �߂�l		0=	����
//			   -1=	���s
//===================================================================
int APIENTRY CTDwFreeCallBack(void)
{
int bln;

    if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
    bln = ( *pCTDFreeCallBack)();

#if _DEBUG 
	DebugPrint("FreeCallBack \n");
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
    return bln;
}

//==============================================================================
//					PMC�R�}���h�f�[�^��������
//				�����F
//					wBsn	=BSN�ԍ�
//					wAxis	=��
//					bCmd    =PMC�R�}���h
//					dwData  =�������݃f�[�^
//					dwStepNo=�X�e�b�vNo.
//				�߂�l�F
//					TRUE or FALSE
//===============================================================================

BOOL APIENTRY CTDwPDataWrite(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPDataWrite)(wBsn,wAxis,bCmd,dwData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PDataWrite wBsn= %d wAxis= %d bCmd= %d dwData= %d dwStepNo= %d \n",
						wBsn, wAxis, bCmd, dwData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					�f�[�^���[�h
//				�����F
//					wBsn	  =BSN�ԍ�
//					wAxis	  =��
//					bCmd      =PMC�R�}���h
//					dwANDData =�ǂݍ��݃f�[�^��AND���^�ł���Ύ��X�e�b�v
//					dwStepNo  =�X�e�b�vNo.
//				�߂�l�F
//					TRUE or FALSE
//===============================================================================

BOOL APIENTRY CTDwPDataRead(WORD wBsn,WORD wAxis,BYTE bCmd,DWORD dwANDData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPDataRead)(wBsn,wAxis,bCmd,dwANDData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PDataRead wBsn= %d wAxis= %d bCmd= %d dwANDData= %d dwStepNo= %d \n",
						wBsn, wAxis, bCmd, dwANDData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}

//==============================================================================
//					Drive �f�[�^���[�h
//				�����F
//					wBsn	  =BSN�ԍ�
//					wAxis	  =��
//					bSetData  =0bit:ON 1���h���C�u�I��
//							   1bit:ON 2���h���C�u�I��
//							   2bit:ON 3���h���C�u�I��
//							   3bit:ON 4���h���C�u�I��
//							   4bit:ON ��Ԋ��荞��
//							�@�@����SetData����v�����玟�X�e�b�v
//					dwStepNo  =�X�e�b�vNo.
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPDriveRead(WORD wBsn,WORD wAxis,BYTE bSetData, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPDriveRead)(wBsn,wAxis,bSetData,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "POutPort wBsn= %d wAxis= %d bSetData= %d dwStepNo= %d \n",
						wBsn, wAxis, bSetData, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}

//==============================================================================
//					PMC Port��������
//				�����F
//					wBsn	  =BSN�ԍ�
//					wAxis	  =��
//					bAdr      =Port Address
//					bdata     =�������݃f�[�^
//					dwStepNo  =�X�e�b�vNo.
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPOutPort(WORD wBsn,WORD wAxis,BYTE bAdr,BYTE bdata, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPOutPort)(wBsn,wAxis,bAdr,bdata,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "POutPort wBsn= %d wAxis= %d bAdr= %d bdata= %d dwStepNo= %d \n",
						wBsn, wAxis, bAdr, bdata, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					PMC Port�ǂݍ���
//				�����F
//					wBsn	  =BSN�ԍ�
//					wAxis	  =��
//					bIoAdr    =Port Address
//					bCmpData  =��r�f�[�^
//					bActiveLevel =bCmpData��r�������ʁA���X�e�b�v�ֈڍs����A�N�e�B�u���x��
//					bCondition =0:������v�Ŏ��X�e�b�v�B��������v���Ȃ��ꍇ�A��������v����܂�wait
//								1:������v�Ŏ��X�e�b�v�B��������v���Ȃ��ꍇ�AdwJmpStepNo�փW�����v
//					dwJmpStep  =�W�����v��X�e�b�vNo.
//					dwStepNo  =�X�e�b�vNo.
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPInPort(WORD wBsn,WORD wAxis,BYTE bIoAdr,BYTE bCmpData,BYTE bActiveLevel,BYTE bCondition,DWORD dwJmpStepNo, DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPInPort)(wBsn,wAxis,bIoAdr,bCmpData,bActiveLevel,bCondition,dwJmpStepNo,dwStepNo) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}

#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PInPort wBsn= %d wAxis= %d bIoAdr= %d bCmpData= %d bActiveLevel= %d bCondition=%d dwJmpStepNo=%d dwStepNo=%d \n",
						wBsn, wAxis, bIoAdr, bCmpData, bActiveLevel, bCondition, dwJmpStepNo, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					�v���O��������J�n�E�I��
//				�����F
//					wBsn	=BSN�ԍ�
//					bSS		=1:�v���O�����J�n 0:�v���O�����I��
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGStartStop(WORD wBsn,BYTE bSS  )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPGStartStop)(wBsn,bSS) == FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
				
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGStartStop wBsn= %d bSS=%d\n",wBsn, bSS);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					�v���O�������X�e�b�v��
//				�����F
//					wBsn	=BSN�ԍ�
//					dwStepNo=���X�e�b�v��
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGAllStep(WORD wBsn,DWORD dwStepNo  )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPGAllStep)(wBsn,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
				
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGAllStep wBsn= %d dwStepNo=%d\n",wBsn, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					Wait����
//				�����F
//					wBsn	  =BSN�ԍ�
//					dwWaitTime=Wait����
//					dwStepNo  =�X�e�b�v�ԍ�
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGWait(WORD wBsn,DWORD dwWaitTime,DWORD dwStepNo )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPGWait)(wBsn,dwWaitTime,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
		
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGWait wBsn= %d dwWaitTime=%d dwStepNo=%d\n",wBsn, dwWaitTime, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					�v���O�����I��
//				�����F
//					wBsn	  =BSN�ԍ�
//					dwStepNo  =�I���X�e�b�v�ԍ�
//				�߂�l�F
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGEnd(WORD wBsn,DWORD dwStepNo)
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPGEnd)(wBsn,dwStepNo)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
			
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGEnd wBsn= %d dwStepNo=%d\n",wBsn, dwStepNo);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//==============================================================================
//					�v���O���� Status
//				�����F
//					wBsn	  =BSN�ԍ�
//				�߂�l�F
//					dwStepNo  =���s���X�e�b�v�ԍ�
//					pbStatePg =0:NO RUN 1:RUNNING
//					TRUE or FALSE
//
//===============================================================================

BOOL APIENTRY CTDwPGStatus(WORD wBsn,PDWORD pdwStepNo, PBYTE pbStatePg)
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDPGStatus)(wBsn,pdwStepNo,pbStatePg)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
			
#if _DEBUG
	memset(buffx, 0, sizeof(buffx));
	sprintf((char*)buffx, "PGStatus wBsn= %d pdwStepNo=%d pbStatePg=%d\n",wBsn, *pdwStepNo, *pbStatePg);
	DebugPrint(buffx);
#endif

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;

}

//===================================================================
// ��  ��		�X�^�[�g�M���̐���
// ��  ��		wBsn		BSN
//				wAxis1Start	���䎲�P,�Q�̓����X�^�[�g�M��
//							1�� ON =1 OFF =0 (0bit)
//						    2�� ON =1 OFF =0 (1bit)
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwStartSignalWrite(WORD wBsn, WORD wStart)
{
	if ( hDLL == NULL ) return FALSE;
	

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDStartSignalWrite)(wBsn,wStart)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}
//===================================================================
// ��  ��		�S���̃X�e�[�^�X���擾
// ��  ��		wBsn	BSN
//				pbData	�ǂݍ��񂾃f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		TRUE	����
//				FALSE	���s
//===================================================================
BOOL APIENTRY CTDwGetAxisStatus( WORD wBsn, PBYTE pbData )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDGetAxisStatus)(wBsn,pbData)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}
//===================================================================
// ��  ��		�w�莲�̊e�X�e�[�^�X���擾
// ��  ��		wBsn		BSN
//				wAxis		�Ώې��䎲
//				pdwData		�ǂݍ��񂾃f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		TRUE		����
//				FALSE		���s
//===================================================================
BOOL APIENTRY CTDwGetAxisAllPort( WORD wBsn, WORD wAxis, PDWORD pdwData )
{
	if ( hDLL == NULL ) return FALSE;

	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
	if((*pCTDGetAxisAllPort)(wBsn,wAxis,pdwData)== FALSE){
				CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
				return FALSE;
	}
				
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return TRUE;
}



//API�֐�

//===================================================================
// ��  ��		�A���[����~�@�\�̗L��/�����ݒ�A���~�b�g���͒�~�ݒ�AINPOSITION WAIT MODE�ݒ�AALARM STOP ENABLE MODE�ݒ�
// ��  ��		wBsn			�Ώۂa�r�m
//				wAxis			�Ώې��䎲
//				bControl		�ݒ�f�[�^
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSetControl( WORD wBsn, WORD wAxis, BYTE bControl )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�A���[����~�@�\�̗L��/�����A���~�b�g���͒�~�AINPOSITION WAIT MODE�AALARM STOP ENABLE MODE�ǂݏo��
// ��  ��		wBsn			�Ώۂa�r�m
//				wAxis			�Ώې��䎲
//				pbControl		�Ǐo�f�[�^
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetControl( WORD wBsn, WORD wAxis, PBYTE pbControl )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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

	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�\�t�g���~�b�g�ݒ�
// ��  ��		wBsn			�a�r�m
//				wAxis			�Ώې��䎲
//				dwHighLimit		����ʒu
//				dwLowLimit		�����ʒu
//				wMode			�\�t�g���~�b�g�@�\�̗L���A�����̐ݒ�B
//								0�F�����\�t�g���~�b�g�L��
//								1�F����ʒu�\�t�g���~�b�g�����A�����ʒu�\�t�g���~�b�g�L��
//								2�F����ʒu�\�t�g���~�b�g�L���A�����ʒu�\�t�g���~�b�g����
//								3�F�����\�t�g���~�b�g����
//				wEvent			�o�͏����ݒ�
//								0�F�Y������������~
//								1�F�Y�������}��~
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSetSoftLimit( WORD wBsn, WORD wAxis, DWORD dwHighLimit, DWORD dwLowLimit, WORD wMode, WORD wEvent )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�\�t�g���~�b�g�擾
// ��  ��		wBsn			�a�r�m
//				wAxis			�Ώې��䎲
//				pdwHighLimit	����ʒu
//				pdwLowLimit		�����ʒu
//				pwMode			�\�t�g���~�b�g�@�\�̗L���A����  
//								0�F�����\�t�g���~�b�g�L��
//								1�F����ʒu�\�t�g���~�b�g�����A�����ʒu�\�t�g���~�b�g�L��
//								2�F����ʒu�\�t�g���~�b�g�L���A�����ʒu�\�t�g���~�b�g����
//								3�F�����\�t�g���~�b�g����
//				pwEvent			�o�͏���
//								0�F�Y������������~
//								1�F�Y�������}��~
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetSoftLimit( WORD wBsn, WORD wAxis, PDWORD pdwHighLimit, PDWORD pdwLowLimit, PWORD pwMode, PWORD pwEvent )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		DLL,�h���C�o�o�[�W�����ǂݏo��
// ��  ��		wBsn			�a�r�m
//				pbDLLVersion	DLL�o�[�W���������i�[����̈�ւ̃|�C���^
//				pbDrvVersion	�h���C�o�o�[�W���������i�[����̈�ւ̃|�C���^
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetVersion( PBYTE pbDLLVersion, PBYTE pbDrvVersion ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		USPG-28L�̃X�e�[�^�X���擾
// ��  ��		wBsn			�a�r�m
//				wAxis			�Ώې��䎲
//				pbStatus		PXPG-48/PXPG-88�̃X�e�[�^�X���i�[����̈�ւ̃|�C���^	
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetBoardStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus )
{
WORD Result;
	if (hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
//			�e��J�E���^�̌��݃J�E���^�l���擾
// ��  ��	wBsn				�a�r�m
//			wAxis				�Ώې��䎲
//			bMode				0:�o�̓p���X�J�E���^
//								1:���g�p
//								2:�h���C�u�p���X�J�E���^
//								3:���g�p
//			pdwData				�w��J�E���^�̒l���i�[����̈�ւ̃|�C���^�B
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetCounter( WORD wBsn, WORD wAxis, BYTE bMode, PDWORD pdwData )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�X�s�[�h�p�����[�^�ݒ�B
// ��  ��		wBsn		�a�r�m
//				wAxis		�Ώې��䎲
//				pSPDData	�X�s�[�h�p�����[�^�f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�X�s�[�h�p�����[�^�擾�B
// ��  ��		wBsn		�a�r�m
//				wAxis		�Ώې��䎲
//				pSPDData	�X�s�[�h�p�����[�^�f�[�^���i�[����̈�ւ̃|�C���^
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetSpeedParameter( WORD wBsn, WORD wAxis, PCTDSPDPARAMETER pSPDData )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�h���C�u���s���A�ڕW���x�̕ύX
// ��  ��		wBsn		�Ώۂa�r�m
//				wAxis		�Ώې��䎲
//				dwHighSpeed	�ύX������̖ڕW���x
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSpeedOverride( WORD wBsn, WORD wAxis, DWORD dwHighSpeed )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		���ݑ��x���擾
// ��  ��		wBsn			�Ώۂa�r�m
//              wAxis			�Ώې��䎲
//              pdwNowSpeed		���ݑ��x���i�[����̈�ւ̃|�C���^�B
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetNowSpeed( WORD wBsn, WORD wAxis, PDWORD pdwNowSpeed )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�h���C�u���s
// ��  ��		wBsn	�Ώۂa�r�m
//              wAxis	�Ώې��䎲
//              bMode	�h���C�u�̎�ނ��w��
// 						0:���Έʒu �ʒu���߃h���C�u
// 						1:��Έʒu �ʒu���߃h���C�u
// 						2:+�����A���h���C�u
//						3:-�����A���h���C�u�@
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwDriveStart( WORD wBsn, WORD wAxis, BYTE bMode, DWORD dwPulse )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�h���C�u��~
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              bStop		0=������~ 1=�}��~
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwDriveStop( WORD wBsn, WORD wAxis, BYTE bStop )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�ړ��ʂ̃I�[�o�[���C�h���s���܂��B
// ��  ��		wBsn		�a�r�m
//				wAxis		�Ώې��䎲
//				dwPulse		�ʒu���߃h���C�u�̎��A�V�����ʒu���W���w�肵�܂��B
//				       		�ݒ�͈́F80000000h�`7FFFFFFFh(�|�Q�P�S�V�S�W�R�U�S�W�`�{�Q�P�S�V�S�W�R�U�S�V) 
//			         		�P    �ʁFPPS
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwPulseOverride( WORD wBsn, WORD wAxis, DWORD dwPulse )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		������������ݒ�B
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              bONOFF		0 = �ҋ@    1 = ���s
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSyncONOFF( WORD wBsn, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		���_���A
// ��  ��		wBsn		�a�r�m
//              wAxis		�Ώې��䎲
//�@�@�@�@�@�@�@bOrgMode	���4bit:���_���A����  ����4bit:���_���A���[�h
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwORGSignalStart( WORD wBsn, WORD wAxis, BYTE bOrgMode )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		���_���A���ɁA�����I�ɒ�~�w�߂𔭍s�B
// ��  ��		wBsn		�a�r�m
//              wAxis		�Ώې��䎲
//�@�@�@�@�@�@�@bStop		���_���A���ɓ��֐����s���A���_���A�������I������ꍇ�A��~���@�ݒ�
//							0 = ������~     1 = �}��~
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwORGSignalStop( WORD wBsn, WORD wAxis, BYTE bStop )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}
	
//===================================================================
// ��  ��		USPG-28L�̌��_���A��Ԃ��擾�B
// ��  ��		wBsn		�a�r�m
//�@�@�@�@�@�@�@wAxis		�Ώې��䎲
//�@�@�@�@�@�@�@pbStatus	���_���A�̏�Ԃ��i�[����̈�ւ̃|�C���^�B0=���_���A�I��  1=���_���A��
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwORGGetStatus( WORD wBsn, WORD wAxis, PBYTE pbStatus )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		���W�ʒu�ݒ�
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              dwPosi		���݈ʒu�f�[�^   �ݒ�͈́F-2,147,483,648 �` +2,147,483,647 �i80000000h �` 7FFFFFFFh�j
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwORGSetPosiData( WORD wBsn, WORD wAxis, DWORD dwPosi )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}
	
//===================================================================
// ��  ��		DRIVE�M���̏�������
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              bONOFF		0:DRIVE�M�� OFF���
//              			1:DRIVE�M�� ON���
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSetDriveONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		DRST�M���̏�������
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              bONOFF		0:DRST�M�� OFF���
//              			1:DRST�M�� ON���
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwSetDrstONOFF( WORD wBsn, WORD wAxis, BYTE bONOFF )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�A���[���R�[�h���擾
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              wValue		0�F���ݔ������̃A���[���R�[�h�擾
//                          1�`14�F�ߋ��ɔ��������A���[��������ǂݏo���܂��B14����ԌÂ������ł��B
//              pbCode		�A���[���R�[�h���i�[����̈�ւ̃|�C���^�i�PByte�f�[�^�j�@
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwGetALMCode( WORD wBsn, WORD wAxis, WORD wValue, PBYTE pbCode ) 
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�A���[���N���A
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              wValue		0�F���ݔ������̃A���[���N���A
//                          1�F�ߋ��ɔ����������ׂẴA���[�������N���A
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwALMCLR( WORD wBsn, WORD wAxis, WORD wValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		�p�����[�^�֘A����
// ��  ��		wBsn 			�Ώۂa�r�m
//              wAxis			�Ώې��䎲
//              bType			0:�p�����[�^������  1:�p�����[�^�Ǐo��
//              wClassNo		�p�����[�^����
//              wParaNo			�p�����[�^No
//              dwData			�p�����[�^�����ݎ��A�����݃f�[�^�ݒ�
//              pdwValue		�p�����[�^�Ǐo�����A�擾�����p�����[�^�l���i�[����̈�ւ̃|�C���^�B
//								�p�����[�^�����ݎ��A�0���Ԃ��܂��B
// �߂�l		0				�f�t�H���g�l
//				1				����I��
//				����ȊO		�G���[�R�[�h
//===================================================================
int APIENTRY CTDwParaCont( WORD wBsn, WORD wAxis, BYTE bType, WORD wClassNo, WORD wParaNo, DWORD dwData, PDWORD pdwValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}

//===================================================================
// ��  ��		���j�^�֘A����
// ��  ��		wBsn		�Ώۂa�r�m
//              wAxis		�Ώې��䎲
//              bkind		1:���j�^�֘A����
//              wCmd		�\�t�g�E�F�A�}�j���A���Q��
//              wValue		�\�t�g�E�F�A�}�j���A���Q��
//              pdwValue	���j�^�f�[�^���i�[����̈�ւ̃|�C���^�B
// �߂�l		0			�f�t�H���g�l
//				1			����I��
//				����ȊO	�G���[�R�[�h
//===================================================================
int APIENTRY CTDwEvent( WORD wBsn, WORD wAxis, BYTE bKind, WORD wCmd, WORD wValue, PDWORD pdwValue )
{
WORD Result;
	if ( hDLL == NULL ){
		Result = CTD_ERR_NOT_OPEN;
		return Result;
	}
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
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
	CTDwLock();	//2014.12.17 �ی�Ώۂ̃R�[�h�ɓ���B
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
	CTDwUnLock();	//2014.12.17 ������A�ʂ̃X���b�h�������悤�ɂ���
	return Result;
}











//-------------------------------------------------------------------
// ��  ��		DLL�G�N�X�|�[�g�֐��A�h���X�̎擾
// ��  ��		�Ȃ�
// �߂�l		TRUE	����
//				FALSE	���s
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
	
	//API�֐�
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

	return TRUE;	// �S�ẴA�h���X�擾����
}

