
#ifdef __BORLANDC__ 
	#define DWORD   DWORD
	#define PDWORD  DWORD *
	#define SDWORD  SDWORD
#else
	#if defined(_MSC_VER) && (_MSC_VER >= 1400) //VS2005
		#define DWORD   unsigned int
		#define PDWORD  unsigned int *
		#define SDWORD  signed int
	#else 
		#define DWORD   unsigned long
		#define PDWORD  unsigned long *
		#define SDWORD  signed long
	#endif
#endif

#define AXIS_POINT	4			/* ���� = 4��	*/
#define AXIS_PTN	1			//�d�l��[0]:1-4�� 
#define AXIS_XY     2			//[0]:X�� [1]:Y��
#define AXIS_4		4

//typedef struct {
//    DWORD   dwBsn;
//    DWORD   dwAxis;
//	DWORD	dwDriveStatus;
//	DWORD	dwEndStatus;
//	DWORD	dwMechanical;
//	DWORD	dwUniversal;
//} CTDIRQINF, *PCTDI;


//typedef struct {
//	unsigned short	wAxis;		// �Ώې��䎲
//	unsigned short	wPort;		// �Ώۃ|�[�g�ԍ�
//	unsigned long	dwData;		// Read/Write Data
//} CTD_IOPARAMS, *pCTD_IOPARAMS;
//
//// Drive parameter
//typedef struct {
//	DWORD	dwLowSpeed;						// ���N�����x(PPS)
//	DWORD	dwHiSpeed;						// �ō����x(PPS)
//	DWORD	dwRate;							// ����������(msec)
//	WORD	wSRate;							// �r��������(%)
//} CTDPARAMETER, *PCTDP;


typedef struct{
	DWORD	dwMode;			//���������[�h
	DWORD	dwLowSpeed;		//�J�n���x
	DWORD	dwHighSpeed;	//�ڕW���x
	DWORD	dwUPTime;		//��������
	DWORD	dwDWTime;		//��������
	DWORD	dwUPSRate;		//����S���䗦
	DWORD	dwDWSRate;		//����S���䗦
	BYTE	bOverONOFF;		//���x�I�[�o�[���C�h�{��
    DWORD	dwUpPls;
    DWORD	dwDownPls;    
} CTDSPDPARAMETER, *PCTDSPDPARAMETER;


// Error code
#define CTD_SUCCESS						0	// �ُ�Ȃ��i����I���j
#define CTD_ERR_SYSTEM						1	// �V�X�e���G���[
													// Windows �� GetLastError()
													// ���R�[�����Ă�������
#define CTD_ERR_NO_DEVICE					2	// �g�p�\�ȃf�o�C�X������܂���
#define CTD_ERR_IN_USE						3	// �w��̃f�o�C�X�͎g�p���ł�
#define CTD_ERR_INVALID_BSN					4	// ������ BSN �ł�
#define CTD_ERR_INVALID_PORT				6	// �s���ȃ|�[�g��v������
//#define CTD_ERR_PARAMETER					7	// �����̒l�������ł�
#define CTD_ERR_PROC						8	// �֐��̎g�p���@���Ԉ���Ă��܂�
#define CTD_ERR_USER_HANDLER				9	// ���[�U�n���h�������s���ł�
#define CTD_ERR_INVALID_AXIS				50	// �����Ȑ��䎲��v������
#define	CTD_ERR_IP							20	// ��ԃG���[
#define CTD_ERR_AXIS_MOVE					102	// �����쒆

#define CTD_MAX_SLOTS						16

#define CTD_ERR_NOT_OPEN					0x1001	// ��������
#define CTD_ERR_NOT_DEVICE					0x1002	// �f�o�C�X�����݂��܂���
#define CTD_ERR_EXIST_OPEN					0x1003	// CTDCreate�Ŋ��ɏ��������Ă��܂��B
#define CTD_INVALID_BSN						0x1010	// �w��BSN�̃{�[�h�����݂��܂���B
#define CTD_INVALID_AXIS					0x1011	// �����Ȑ��䎲k��v��
#define CTD_INVALID_DATA					0x1012	// �f�[�^���ݒ�͈͊O�B�f�[�^�G���[�B
#define CTD_ERR_MINUS_SET					0x1020	// �ʒu���߃h���C�u���s����CTDSpeedOverride�Ń}�C�i�X�l�̕ύX���x��ݒ肵���B
#define CTD_ERR_AXIS_STOP					0x1021	// �h���C�u���ɔ��s����֐����h���C�u��~���ɔ��s�����B
//#define CTD_ERR_AXIS_MOVE					0x1022	// �����쒆�B
#define CTD_ERR_DRIVE_START					0x1023	// CTDDriveStart���s���̃G���[�B
#define CTD_ERR_DRIVE_KIND					0x1024	// �A���h���C�u����CTDPulseOverride�𔭍s�����B
#define CTD_ERR_PARAMETER					0x1025	// CTDParaCont �p�����[�^���ށA�p�����[�^No.���͈͊O�B
#define CTD_ERR_ORG							0x1030	// CTDORGSignalStart�𔭍s���Ă����[�^����]���Ȃ��B


// Axis
#define CTD_AXIS_1							0
#define CTD_AXIS_2							1
#define CTD_AXIS_3							2
#define CTD_AXIS_4							3


// Coordinates classification
#define CTD_MOVE_KIND_INC					0
#define CTD_MOVE_KIND_ABS					1

// I/O Map
#define CTD_PORT_DATA1							0x00
#define CTD_PORT_DATA2							0x01
#define CTD_PORT_DATA3							0x02
#define CTD_PORT_DATA4							0x03
#define CTD_PORT_COMMAND						0x04
#define CTD_PORT_MODE1							0x05
#define CTD_PORT_MODE2							0x06
#define CTD_PORT_UNIVERSAL_SIGNAL				0x07
#define CTD_PORT_DRIVE_STATUS					0x04
#define CTD_PORT_END_STATUS						0x05
#define CTD_PORT_MECHANICAL_SIGNAL				0x06

// Commands
#define CTD_RANGE_WRITE							0x00
#define CTD_RANGE_READ							0x01
#define CTD_START_STOP_SPEED_DATA_WRITE			0x02
#define CTD_START_STOP_SPEED_DATA_READ			0x03
#define CTD_OBJECT_SPEED_DATA_WRITE				0x04
#define CTD_OBJECT_SPEED_DATA_READ				0x05
#define CTD_RATE1_DATA_WRITE					0x06
#define CTD_RATE1_DATA_READ						0x07
#define CTD_RATE2_DATA_WRITE					0x08
#define CTD_RATE2_DATA_READ						0x09
#define CTD_SLOW_DOWN_REAR_PULSE_WRITE			0x10
#define CTD_SLOW_DOWN_REAR_PULSE_READ			0x11
#define CTD_NOW_SPEED_DATA_READ					0x12
#define CTD_DRIVE_PULSE_COUNTER_READ			0x13
#define CTD_PRESET_PULSE_DATA_OVERRIDE			0x14
#define CTD_PRESET_PULSE_DATA_READ				0x15
#define CTD_DEVIATION_DATA_READ					0x16
#define CTD_INPOSITION_WAIT_MODE1_SET			0x17
#define CTD_INPOSITION_WAIT_MODE2_SET			0x18
#define CTD_INPOSITION_WAIT_MODE_RESET			0x19
#define CTD_ALARM_STOP_ENABLE_MODE_SET			0x1a
#define CTD_ALARM_STOP_ENABLE_MODE_RESET		0x1b
#define CTD_SLOW_DOWN_STOP						0x1e
#define CTD_EMERGENCY_STOP						0x1f
#define CTD_PLUS_PRESET_PULSE_DRIVE				0x20
#define CTD_MINUS_PRESET_PULSE_DRIVE			0x21
#define CTD_PLUS_CONTINUOUS_DRIVE				0x22
#define CTD_MINUS_CONTINUOUS_DRIVE				0x23
#define CTD_PLUS_SIGNAL_SEARCH1_DRIVE			0x24
#define CTD_MINUS_SIGNAL_SEARCH1_DRIVE			0x25
#define CTD_PLUS_SIGNAL_SEARCH2_DRIVE			0x26
#define CTD_MINUS_SIGNAL_SEARCH2_DRIVE			0x27
#define CTD_INTERNAL_COUNTER_WRITE				0x28
#define CTD_INTERNAL_COUNTER_READ				0x29
#define CTD_INTERNAL_COMPARATE_DATA_WRITE		0x2a
#define CTD_INTERNAL_COMPARATE_DATA_READ		0x2b
#define CTD_EXTERNAL_COUNTER_WRITE				0x2c
#define CTD_EXTERNAL_COUNTER_READ				0x2d
#define CTD_EXTERNAL_COMPARATE_DATA_WRITE		0x2e
#define CTD_EXTERNAL_COMPARATE_DATA_READ		0x2f
#define CTD_INTERNAL_PRE_SCALE_DATA_WRITE		0x30
#define CTD_INTERNAL_PRE_SCALE_DATA_READ		0x31
#define CTD_EXTERNAL_PRE_SCALE_DATA_WRITE		0x32
#define CTD_EXTERNAL_PRE_SCALE_DATA_READ		0x33
#define CTD_CLEAR_SIGNAL_SELECT					0x34
#define CTD_ONE_TIME_CLEAR_REQUEST				0x35
#define CTD_FULL_TIME_CLEAR_REQUEST				0x36
#define CTD_CLEAR_REQUEST_RESET					0x37
#define CTD_REVERSE_COUNT_MODE_SET				0x38
#define CTD_REVERSE_COUNT_MODE_RESET			0x39
#define CTD_NO_OPERATION						0x3a
#define CTD_STRAIGHT_ACCELERATE_MODE_SET		0x84
#define CTD_US_STRAIGHT_ACCELERATE_MODE_SET		0x85
#define CTD_S_CURVE_ACCELERATE_MODE_SET			0x86
#define CTD_US_S_CURVE_ACCELERATE_MODE_SET		0x87
#define CTD_SW1_DATA_WRITE						0x88
#define CTD_SW1_DATA_READ						0x89
#define CTD_SW2_DATA_WRITE						0x8a
#define CTD_SW2_DATA_READ						0x8b
#define CTD_SLOW_DOWN_LIMIT_ENABLE_MODE_SET		0x8c
#define CTD_SLOW_DOWN_LIMIT_ENABLE_MODE_RESET	0x8d
#define CTD_EMERGENCY_LIMIT_ENABLE_MODE_SET		0x8e
#define CTD_EMERGENCY_LIMIT_ENABLE_MODE_RESET	0x8f
#define CTD_INITIAL_CLEAR						0x90
#define CTD_MODE1_READ							0xa0
#define CTD_MODE2_READ							0xa1
#define CTD_STATUS_READ							0xa2
#define CTD_SLOW_DOWN_STOP2						0xa3
#define CTD_RISE_PULSE_COUNTER_READ				0xa6
#define CTD_BI_PHASE_PULSE_SELECT_DATA_WRITE	0xaa
#define CTD_BI_PHASE_PULSE_SELECT_DATA_READ		0xab
#define CTD_SIGNAL_SERH2_REAR_PULSE_DATA_WRITE	0xac
#define CTD_SIGNAL_SERH2_REAR_PULSE_DATA_READ	0xad
#define CTD_MANUAL_PULSE_MODE_WRITE				0xc0
#define CTD_MANUAL_PULSE_MODE_READ				0xc1
#define CTD_SOFT_SYNC_MODE_WRITE				0xc2
#define CTD_SOFT_SYNC_MODE_READ					0xc3
#define CTD_SOFT_SYNC_EXECUTE					0xc4


//============================================
//			���x�֘A�p�����[�^
//============================================
#define MAX_AXIS 			4
#define CTD_CAUTION	2
#define	BAS_RNG	 			(32768000.0 / 32768.0)
#define	BAS_RTF	 			(32768000.0 / 8.0)
#define	BAS_RTF2			(32768000.0 / 16.0)
#define	RATEDAT				(8.0 / 32768000.0)
#define	CTD_STRAIGHT_MOVE		-1

/* SpeedParameterRead�Őݒ肳�ꂽ�p�����[�^ */
typedef struct{
	double			dLoSpd[MAX_AXIS];
	double			dHiSpd[MAX_AXIS];
	double			dObjSpd[MAX_AXIS];
	short			sUpDnSpd[MAX_AXIS];
	double			dSCurve[MAX_AXIS];
} CTD_SPD_PARAM, *pCTD_SPD_PARAMS;

//===========================================
//				��ԋ@�\
//===========================================
typedef struct{

	char   bIpAxis[AXIS_PTN][AXIS_4];		// ��Ԏ��̐ݒ� AXIS_PTN=[0]:1-4�� 
                                            // AXIS_4=[0]:1����-[3]:4���� �@
											// ���e:0=���� 1=X�� 2=Y�� -1=�o�͂Ȃ�
	unsigned char	bIpkind[AXIS_PTN];		// ��Ԏ��	AXIS_PTN=[0]:1-4�� 
	/* = 0:�������							*/
	/* = 1:�������(�����␳����)			*/
	/* = 2:�E���~�ʕ��(�����␳�Ȃ�)		*/
	/* = 3:�����~�ʕ��(�����␳�Ȃ�)		*/
	/* = 4:�E���~�ʕ��(�����␳����)		*/
	/* = 5:�����~�ʕ��(�����␳����)		*/

	//*05.09.14 lLongData�̈��n���폜

	unsigned char   bMovekind[AXIS_PTN];	         // 0=���Έړ��@1=��Έړ�		    
	long			lObjPoint[AXIS_PTN][AXIS_XY];	 // �ڕW�ʒu AXIS_XY=[0]:X�� [1]:Y��
	long			lCenterPoint[AXIS_PTN][AXIS_XY]; // ���S�ʒu AXIS_XY=[0]:X�� [1]:Y��						
	long			lN2Data1[AXIS_PTN];				 //	2,6�� N2�f�[�^
	long			lN2Data2[AXIS_PTN];				 // 3,7�� N2�f�[�^
	long			lN2Data3[AXIS_PTN];				 // 4,8�� N2�f�[�^			    

	unsigned char	bIpconsin[AXIS_PTN];		 	 // �A��:1 �P��:0
	unsigned char   bEndkind[AXIS_PTN];		 		 // SlowDown:0,Emergency Stop:1
	unsigned char   bDrawIn[AXIS_PTN];		 		 // �������ݐ��� 0=���� 1=�L��

}CTIpDriveParameter,*pCTID;

