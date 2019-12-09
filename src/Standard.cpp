#include "../inc/Standard.hpp"

/*-----------------------------------------------
*
* エラーメッセージ
*
-----------------------------------------------*/
void PrintError(const char *msg)
{
	MessageBox(NULL, _T(msg), _T("Error"), MB_OK | MB_ICONSTOP);
}