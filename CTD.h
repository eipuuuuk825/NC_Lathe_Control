
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

#define AXIS_POINT	4			/* 軸数 = 4軸	*/
#define AXIS_PTN	1			//仕様＝[0]:1-4軸 
#define AXIS_XY     2			//[0]:X軸 [1]:Y軸
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
//	unsigned short	wAxis;		// 対象制御軸
//	unsigned short	wPort;		// 対象ポート番号
//	unsigned long	dwData;		// Read/Write Data
//} CTD_IOPARAMS, *pCTD_IOPARAMS;
//
//// Drive parameter
//typedef struct {
//	DWORD	dwLowSpeed;						// 自起動速度(PPS)
//	DWORD	dwHiSpeed;						// 最高速度(PPS)
//	DWORD	dwRate;							// 加減速時間(msec)
//	WORD	wSRate;							// Ｓ字加減率(%)
//} CTDPARAMETER, *PCTDP;


typedef struct{
	DWORD	dwMode;			//加減速モード
	DWORD	dwLowSpeed;		//開始速度
	DWORD	dwHighSpeed;	//目標速度
	DWORD	dwUPTime;		//加速時間
	DWORD	dwDWTime;		//減速時間
	DWORD	dwUPSRate;		//加速S字比率
	DWORD	dwDWSRate;		//減速S字比率
	BYTE	bOverONOFF;		//速度オーバーライド倍数
    DWORD	dwUpPls;
    DWORD	dwDownPls;    
} CTDSPDPARAMETER, *PCTDSPDPARAMETER;


// Error code
#define CTD_SUCCESS						0	// 異状なし（正常終了）
#define CTD_ERR_SYSTEM						1	// システムエラー
													// Windows の GetLastError()
													// をコールしてください
#define CTD_ERR_NO_DEVICE					2	// 使用可能なデバイスがありません
#define CTD_ERR_IN_USE						3	// 指定のデバイスは使用中です
#define CTD_ERR_INVALID_BSN					4	// 無効な BSN です
#define CTD_ERR_INVALID_PORT				6	// 不正なポートを要求した
//#define CTD_ERR_PARAMETER					7	// 引数の値が無効です
#define CTD_ERR_PROC						8	// 関数の使用方法が間違っています
#define CTD_ERR_USER_HANDLER				9	// ユーザハンドラが実行中です
#define CTD_ERR_INVALID_AXIS				50	// 無効な制御軸を要求した
#define	CTD_ERR_IP							20	// 補間エラー
#define CTD_ERR_AXIS_MOVE					102	// 軸動作中

#define CTD_MAX_SLOTS						16

#define CTD_ERR_NOT_OPEN					0x1001	// 未初期化
#define CTD_ERR_NOT_DEVICE					0x1002	// デバイスが存在しません
#define CTD_ERR_EXIST_OPEN					0x1003	// CTDCreateで既に初期化しています。
#define CTD_INVALID_BSN						0x1010	// 指定BSNのボードが存在しません。
#define CTD_INVALID_AXIS					0x1011	// 無効な制御軸kを要求
#define CTD_INVALID_DATA					0x1012	// データが設定範囲外。データエラー。
#define CTD_ERR_MINUS_SET					0x1020	// 位置決めドライブ実行中にCTDSpeedOverrideでマイナス値の変更速度を設定した。
#define CTD_ERR_AXIS_STOP					0x1021	// ドライブ中に発行する関数をドライブ停止中に発行した。
//#define CTD_ERR_AXIS_MOVE					0x1022	// 軸動作中。
#define CTD_ERR_DRIVE_START					0x1023	// CTDDriveStart発行時のエラー。
#define CTD_ERR_DRIVE_KIND					0x1024	// 連続ドライブ中にCTDPulseOverrideを発行した。
#define CTD_ERR_PARAMETER					0x1025	// CTDParaCont パラメータ分類、パラメータNo.が範囲外。
#define CTD_ERR_ORG							0x1030	// CTDORGSignalStartを発行してもモータが回転しない。


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
//			速度関連パラメータ
//============================================
#define MAX_AXIS 			4
#define CTD_CAUTION	2
#define	BAS_RNG	 			(32768000.0 / 32768.0)
#define	BAS_RTF	 			(32768000.0 / 8.0)
#define	BAS_RTF2			(32768000.0 / 16.0)
#define	RATEDAT				(8.0 / 32768000.0)
#define	CTD_STRAIGHT_MOVE		-1

/* SpeedParameterReadで設定されたパラメータ */
typedef struct{
	double			dLoSpd[MAX_AXIS];
	double			dHiSpd[MAX_AXIS];
	double			dObjSpd[MAX_AXIS];
	short			sUpDnSpd[MAX_AXIS];
	double			dSCurve[MAX_AXIS];
} CTD_SPD_PARAM, *pCTD_SPD_PARAMS;

//===========================================
//				補間機能
//===========================================
typedef struct{

	char   bIpAxis[AXIS_PTN][AXIS_4];		// 補間軸の設定 AXIS_PTN=[0]:1-4軸 
                                            // AXIS_4=[0]:1軸目-[3]:4軸目 　
											// 内容:0=自軸 1=X軸 2=Y軸 -1=出力なし
	unsigned char	bIpkind[AXIS_PTN];		// 補間種類	AXIS_PTN=[0]:1-4軸 
	/* = 0:直線補間							*/
	/* = 1:直線補間(周速補正あり)			*/
	/* = 2:右回り円弧補間(周速補正なし)		*/
	/* = 3:左回り円弧補間(周速補正なし)		*/
	/* = 4:右回り円弧補間(周速補正あり)		*/
	/* = 5:左回り円弧補間(周速補正あり)		*/

	//*05.09.14 lLongDataの引渡し削除

	unsigned char   bMovekind[AXIS_PTN];	         // 0=相対移動　1=絶対移動		    
	long			lObjPoint[AXIS_PTN][AXIS_XY];	 // 目標位置 AXIS_XY=[0]:X軸 [1]:Y軸
	long			lCenterPoint[AXIS_PTN][AXIS_XY]; // 中心位置 AXIS_XY=[0]:X軸 [1]:Y軸						
	long			lN2Data1[AXIS_PTN];				 //	2,6軸 N2データ
	long			lN2Data2[AXIS_PTN];				 // 3,7軸 N2データ
	long			lN2Data3[AXIS_PTN];				 // 4,8軸 N2データ			    

	unsigned char	bIpconsin[AXIS_PTN];		 	 // 連続:1 単発:0
	unsigned char   bEndkind[AXIS_PTN];		 		 // SlowDown:0,Emergency Stop:1
	unsigned char   bDrawIn[AXIS_PTN];		 		 // 引き込み制御 0=無効 1=有効

}CTIpDriveParameter,*pCTID;

