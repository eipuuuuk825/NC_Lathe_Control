//===================================================================
// CTDs.cpp
//-------------------------------------------------------------------
// USPG-48 ����
// Copyright CosmoTechs Co.,Ltd.
//
// Date 08.2005
//
// ���̃\�[�X�ɂ̓��b�p�[�֐��̎g�p�Ⴊ�L�q����Ă��܂�
//===================================================================
//#include "stdafx.h"	// �l�e�b���g�p�����Ƃ��͂��̍s���K�v�ł�
#include <windows.h>
#include <tchar.h>
#include "CTDw.h"

const TCHAR szTitle[]	= _T("Sample");

static void ResultMessage(LPCTSTR lpszbuf);
static void ErrorMessage(WORD wBsn);
static BOOL CTD_InitAxis(WORD wBsn,WORD wAxis);

//===================================================================
// ���C��
//===================================================================
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//										LPSTR lpCmdLine, int nCmdShow)
int main(void)
{
	TCHAR	szBuf[1024];
	BYTE	bData[256];
	WORD	wBsn, wAxis;
	BYTE	b;
	DWORD	dwCount;

	//---------------------------------------------------------------
	// CTD.Dll ���J��
	//---------------------------------------------------------------
	if ( FALSE == CTDwDllOpen() ) {
		wsprintf(szBuf, _T("DLL load error or driver open error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;		// DLL�̃��[�h�܂��̓h���C�o�̃I�[�v���Ɏ��s���܂���
	}

	//---------------------------------------------------------------
	// �c�k�k�o�[�W�������̕\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetLibVersion(bData)) {
		wsprintf(szBuf, _T("DLL version error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("Dll Version %s"),
		bData );
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �h���C�o�o�[�W�������̕\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetDrvVersion(bData)) {
		wsprintf(szBuf, _T("Driver version error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("Drv Version %s"),
		bData );
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �f�o�C�X�̎g�p��錾����
	//---------------------------------------------------------------
	wBsn = 0;
	if ( -1 == CTDwCreate(wBsn) ){
		wsprintf(szBuf, _T("Create error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD Created\n")
		_T("BSN Number %d"), wBsn );
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// ROM�o�[�W�������̕\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetRomVersion(wBsn,bData)) {
		wsprintf(szBuf, _T("ROM version error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("ROM Version %s"),
		bData );
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// ��P����I��
	//---------------------------------------------------------------
	wAxis =CTD_AXIS_1;
	//---------------------------------------------------------------
	// ���䎲�̏�����
	//---------------------------------------------------------------
	if ( FALSE == CTD_InitAxis(wBsn,wAxis)){
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("���䎲 %d �����������܂���"), wAxis );
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �{������ 1000 �p���X�o��
	//---------------------------------------------------------------
	if ( FALSE == CTDwDataFullWrite( 
		wBsn, wAxis, CTD_PLUS_PRESET_PULSE_DRIVE, 1000 ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("�{������ 1000�p���X�o�͂��Ă��܂�"));
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �h���C�u�X�e�[�^�X���擾
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetDriveStatus( wBsn, wAxis, &b ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("�h���C�u�X�e�[�^�X: %Xh"),b);
	ResultMessage( szBuf );

	//---------------------------------------------------------------
	// �r�W�[�t���O�������Ă�����i�h���C�u���Ȃ�j������~
	//---------------------------------------------------------------
	if ( b & 0x01 ) {
		if ( FALSE == CTDwCommandWrite( wBsn, wAxis, CTD_SLOW_DOWN_STOP ) ){
			ErrorMessage(wBsn);
			return 1;
		}
		wsprintf( szBuf,
			_T("CTD\n")
			_T("������~���܂���"));
		ResultMessage( szBuf );
	}
	//---------------------------------------------------------------
	// �G���h�X�e�[�^�X���擾
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetEndStatus( wBsn, wAxis, &b ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("�G���h�X�e�[�^�X: %Xh"),b);
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �����J�E���^�[��\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetInternalCounter(wBsn, wAxis, &dwCount ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("�����J�E���^�[: %d"),dwCount);
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// ���J�j�J���V�O�i����\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetMechanicalSignal( wBsn, wAxis, &b ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("���J�j�J���V�O�i��: %Xh"),b);
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// ���j�o�[�T���V�O�i����\��
	//---------------------------------------------------------------
	if ( FALSE == CTDwGetUniversalSignal( wBsn, wAxis, &b ) ) {
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD\n")
		_T("���j�o�[�T���V�O�i��: %Xh"),b);
	ResultMessage( szBuf );
	//---------------------------------------------------------------
	// �f�o�C�X���������
	//---------------------------------------------------------------
	if ( FALSE == CTDwClose(wBsn)){
		ErrorMessage(wBsn);
		return 1;
	}
	wsprintf( szBuf,
		_T("CTD Close\n")
		_T("Bsn %d"), wBsn );
	ResultMessage( szBuf );

	//---------------------------------------------------------------
	// CTD.Dll �����
	//---------------------------------------------------------------
	if ( FALSE == CTDwDllClose()) {
		wsprintf(szBuf, _T("DLL Unload error"));
		MessageBox(NULL, szBuf, szTitle, MB_OK | MB_ICONSTOP);
		return 1;						// DLL�̃A�����[�h�Ɏ��s���܂���
	}

	return 0;
}
//-------------------------------------------------------------------
// ���䎲�̏�����
//-------------------------------------------------------------------
// ��  ��		wBsn	�a�r�m�i�{�[�h�Z���N�g�i���o�[�j
//				wAxis	���䎲
//             	CTD_AXIS_1 �` CTD_AXIS_4 ���w�肵�܂�
//-------------------------------------------------------------------
static BOOL CTD_InitAxis(WORD wBsn,WORD wAxis)
{
	//---------------------------------------------------------------
	// MODE1 SET
	//
	// �����J�n�|�C���g���o����				����
	// �p���X�o�͕���						�Q�p���X����
	// DIR   �o�͒[�q 						CW�p���X  �A�N�e�B�u Hi
	// PULSE �o�͒[�q						CCW�p���X �A�N�e�B�u Hi
	//---------------------------------------------------------------
	if ( FALSE == CTDwMode1Write(wBsn,wAxis,0x40)) return FALSE;
	//---------------------------------------------------------------
	// MODE2 SET
	//
	// EXTERNAL COUNTER ���͎d�l			None
	// DEND ���͐M���A�N�e�B�u���x��		High
	// DERR ���͐M���A�N�e�B�u���x��		High
	// -SLM ���͐M���A�N�e�B�u���x��		High
	// +SLM ���͐M���A�N�e�B�u���x��		High
	// -ELM ���͐M���A�N�e�B�u���x��		High
	// +ELM ���͐M���A�N�e�B�u���x��		High
	//---------------------------------------------------------------
	if ( FALSE == CTDwMode2Write(wBsn,wAxis,0x3F)) return FALSE;
	//---------------------------------------------------------------
	// ���[�h�ݒ�
	//
	// INPOSITION WAIT MODE RESET
	// ALARM STOP ENABLE MODE SET
	//---------------------------------------------------------------
	if ( FALSE == CTDwCommandWrite(
		wBsn,wAxis,CTD_INPOSITION_WAIT_MODE_RESET))
		return FALSE;
	if ( FALSE == CTDwCommandWrite(
		wBsn,wAxis,CTD_ALARM_STOP_ENABLE_MODE_SET)) return FALSE;
	//---------------------------------------------------------------
	// �f�[�^�ݒ�
	//
	// RANGE DATA				100		�o�͎��g���ݒ�P�� 	1000��100=10PPS
	// START/STOP SPEED DATA	100		�J�n��~���g�� 		100�~10PPS=1000PPS
	// OBJECT SPEED DATA		4000	�ړI���g�� 			4000�~10PPS=40000PPS
	// RATE-1 DATA              2048	�������Ԑݒ�P�� 	2048��(4.096�~10^6)=0.5mSec
	//									�������� 			(4000-100)�~0.5mSec = 1.95Sec
	// RATE-2 DATA        				�f�t�H���g�l 8191(1FFFh)
	//
	// ���̐ݒ�ɂ�� RATE-1 DATA �ɂ�钼���������ƂȂ�܂�
	//---------------------------------------------------------------
	if ( FALSE == CTDwDataHalfWrite(
		wBsn,wAxis,CTD_RANGE_WRITE,100)) return FALSE;
	if ( FALSE == CTDwDataHalfWrite(
		wBsn,wAxis,CTD_START_STOP_SPEED_DATA_WRITE,100)) return FALSE;
	if ( FALSE == CTDwDataHalfWrite(
		wBsn,wAxis,CTD_OBJECT_SPEED_DATA_WRITE,4000)) return FALSE;
	if ( FALSE == CTDwDataHalfWrite(
		wBsn,wAxis,CTD_RATE1_DATA_WRITE,2048)) return FALSE;
	//---------------------------------------------------------------
	// �A�h���X�ݒ�
	//
	// INTERNAL COUNTER �y�� EXTERNAL COUNTER �� 0h ���������݂܂�
	//---------------------------------------------------------------
	if ( FALSE == CTDwDataFullWrite(
		wBsn,wAxis,CTD_INTERNAL_COUNTER_WRITE,0)) return FALSE;
	if ( FALSE == CTDwDataFullWrite(
		wBsn,wAxis,CTD_EXTERNAL_COUNTER_WRITE,0)) return FALSE;

	return TRUE;
}
//-------------------------------------------------------------------
// �������ʂ�\��
//-------------------------------------------------------------------
static void ResultMessage(LPCTSTR lpszbuf)
{
	MessageBox(NULL, lpszbuf, szTitle, MB_OK | MB_ICONINFORMATION);
}
//-------------------------------------------------------------------
// �G���[���b�Z�[�W��\��
//-------------------------------------------------------------------
static void ErrorMessage(WORD wBsn)
{
	DWORD dwRes=CTDwGetLastError(wBsn); 
	TCHAR szbuf[50];
	
	wsprintf(szbuf, _T("CTD error\n\n")
					_T("Error code: %08lXh"), dwRes);

	MessageBox(NULL, szbuf, szTitle, MB_OK | MB_ICONSTOP);
}
