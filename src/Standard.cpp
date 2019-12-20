#include "../inc/standard.hpp"

/*-----------------------------------------------
*
* エラーメッセージ
*
-----------------------------------------------*/
void print_err(const char *msg)
{
    MessageBox(NULL, _T(msg), _T("Error"), MB_OK | MB_ICONSTOP);
    exit(EXIT_FAILURE);
}

/*-----------------------------------------------
 *
 * 文字列 str を区切り文字 sep で分割する
 *
-----------------------------------------------*/
std::vector<std::string> split(const std::string &str, char sep)
{
    std::vector<std::string> v;
    std::stringstream ss(str);
    std::string buffer;
    while (std::getline(ss, buffer, sep))
    {
        v.push_back(buffer);
    }
    return v;
}