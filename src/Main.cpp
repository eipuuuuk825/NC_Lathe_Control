#include <cstdio>
#include <vector>
#include "../inc/Motor.hpp"

std::vector<short> LoadTargetData(const char *path);

const uint16_t Bsn = 0;
const size_t MaxAxisNum = 4; /* 軸の最大個数 */

int main(void)
{
	/*-----------------------------------------------
	内部カウンタ目標値を読み込む
	-----------------------------------------------*/
	// std::vector<short> TargetData = LoadTargetData("");
	std::vector<int32_t> TargetData{500, -1000, 1000, 2000};

	/*-----------------------------------------------
	初期化
	-----------------------------------------------*/
	MOTOR Motor(Bsn, MaxAxisNum);
	Motor.ChangeSpeed(CTD_AXIS_3, SPEED(300, 1000, 100));

	/*-----------------------------------------------
	リミットスイッチが押されるまで駆動
	-----------------------------------------------*/
	Motor.DriveLimitSwitch(CTD_AXIS_3, PLUS);
	while (Motor.GetBusy(CTD_AXIS_3))
		;
	Motor.SetIntCnt(CTD_AXIS_3, 0);

	/*-----------------------------------------------
	内部カウンタ目標値に合わせてパルスを出力
	-----------------------------------------------*/
	while (!TargetData.empty())
	{
		/* パルス出力 */
		if (!Motor.GetBusy(CTD_AXIS_3))
		{
			printf("Target %+5d\tCurrent %+5d\n",
				   TargetData[0], Motor.GetIntCnt(CTD_AXIS_3));
			Motor.DriveIntCnt(CTD_AXIS_3, TargetData[0]);
			TargetData.erase(TargetData.begin());
		}
		/* キー入力 */
		if (GetAsyncKeyState('Q'))
			break;
	}

	return 0;
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
		TargetData.emplace_back((i + 1) * -10);
	for (size_t i = 0; i < TargetData.size(); i++)
		printf("%d\n", TargetData[i]);
	return TargetData;
}