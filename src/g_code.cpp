#include "../inc/g_code.hpp"

Speed g_interpolation_speed(-1, -1, -1);
double g_spindle_speed;
std::vector<Queue> g_q;

void analyze_gcode(const char *file_path)
{
    /* ファイルから読み込む */
    std::string str_raw = read_file(file_path);

    /* タグ毎に分ける */
    std::vector<std::string> str_splited = split_tag(str_raw);

    /* タグごとの処理 */
    for (size_t i = 0; i < str_splited.size(); i++)
        exe_analyze(str_splited[i]);
}

/*-----------------------------------------------
ファイルから読み込む
-----------------------------------------------*/
std::string read_file(const char *file_path)
{
    /* ファイルを開く */
    std::ifstream file(file_path, std::ios::in);
    if (file.fail())
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nopen error [" << file_path << "]" << std::endl;
        print_err(sstr.str().c_str());
    }

    /* ファイルから読み込む */
    std::string str_raw((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    /* ファイルを閉じる */
    file.close();

    return str_raw;
}

/*-----------------------------------------------
タグ毎に分ける
-----------------------------------------------*/
std::vector<std::string> split_tag(std::string str_raw)
{
    std::vector<std::string> str_splited; /* タグ毎に分けた後の文字列ベクタ */
    std::string str_buf;

    for (size_t i = 0; i < str_raw.size(); i++)
    {
        /* タグだったらその時点での str_buf を str_splited に追加 */
        if (is_tag(str_raw[i]))
        {
            if (!str_buf.empty())
            {
                str_splited.emplace_back(str_buf);
                str_buf.clear();
            }
            str_buf.push_back(str_raw[i]);
        }
        /* タグではなかったら str_buf に追加 */
        else
        {
            str_buf.push_back(str_raw[i]);
        }
    }

    /* 最後のタグを追加 */
    if (is_tag(str_buf[0]))
        str_splited.emplace_back(str_buf);

    return str_splited;
}

/*-----------------------------------------------
引数に渡した文字がタグかどうか判断する
-----------------------------------------------*/
bool is_tag(char tag)
{
    for (size_t i = 0; i < g_tag.size(); i++)
    {
        if (g_tag[i].m_tag_char == tag)
            return true;
    }
    return false;
}

/*-----------------------------------------------
各タグ毎の解析を実行する
-----------------------------------------------*/
void exe_analyze(std::string str)
{
    char tag = str[0];
    for (size_t i = 0; i < g_tag.size(); i++)
    {
        if (tag == g_tag[i].m_tag_char)
        {
            g_tag[i].m_tag_func(str);
            return;
        }
    }

    /* タグが見つからなかったらエラー */
    std::stringstream sstr;
    sstr << __FUNCSIG__ << "\n\ntag : " << str[0] << std::endl;
    print_err(sstr.str().c_str());
}

/*-----------------------------------------------
*
* タグ毎の処理
*
-----------------------------------------------*/
void analyze_G(std::string str)
{
    /* エラーチェック */
    if (str.size() < 3)
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* G00 か G01 かを判断（速度） */
    analyze_G_speed(str);

    /* X, Z（目標位置） */
    analyze_G_pos(str);
}

/*-----------------------------------------------
G の速度
-----------------------------------------------*/
void analyze_G_speed(std::string str)
{
    static Speed pre_speed;
    Speed target_speed;
    int number = std::stoi(str.substr(1, 2));

    /* 速度を設定 */
    switch (number)
    {
    case 0:
        target_speed = g_fast_forward;
        break;
    case 1:
        target_speed = g_interpolation_speed;
        break;
    default:
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* キューに追加 */
    if (target_speed != pre_speed)
    {
        Queue tmp(MODE_CHANGE_SPEED_SPTEPPING_MOTOR, target_speed);
        g_q.emplace_back(tmp);
    }

    /* 更新 */
    pre_speed = target_speed;
}

/*-----------------------------------------------
G の位置
-----------------------------------------------*/
void analyze_G_pos(std::string str)
{
    Pos target_pos;
    for (size_t i = 0; i < 2; i++)
    {
        double pos;
        size_t index = (i == 0) ? str.find("X") : str.find("Z");
        /* X or Z が見つからなかった場合，前の値から探す */
        if (index == std::string::npos)
        {
            if (i == 0)
                pos = find_previous_pos(g_q.size(), 'X');
            else
                pos = find_previous_pos(g_q.size(), 'Z');
        }
        /* 見つかった場合 */
        else
        {
            std::string read_data;
            for (size_t j = index + 1; j < str.size(); j++)
            {
                if (is_numeric(str[j]))
                    read_data.push_back(str[j]);
                else
                    break;
            }
            pos = std::stod(read_data);
        }

        if (i == 0)
            target_pos.m_x = pos;
        else
            target_pos.m_z = pos;
    }

    /* キューに追加 */
    Queue tmp(MODE_DRIVE_STEPPING_MOTOR, target_pos);
    g_q.emplace_back(tmp);
}

void analyze_M(std::string str)
{
    /* エラーチェック */
    if (str.size() < 3)
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* M コードの番号を判断 */
    int number = std::stoi(str.substr(1, 2));

    /* M コードの種類に応じた処理 */
    Queue tmp;
    switch (number)
    {
    case 3:
        tmp.m_mode = MODE_DRIVE_SERVO;
        break;
    case 5:
        tmp.m_mode = MODE_STOP_SERVO;
        break;
    case 2:
    case 30:
        break;
    default:
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }
    g_q.emplace_back(tmp);
}

void analyze_S(std::string str)
{
    /* エラーチェック１ */
    static size_t call_cnt;
    ++call_cnt;
    if (call_cnt > 1)
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nDon't change speed twice.\n"
             << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* エラーチェック２ */
    if (str.size() <= 1)
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* 回転数を取得 */
    int rpm = std::stoi(str.substr(1, str.size() - 1));

    /* キューに追加 */
    Queue tmp(MODE_CHANGE_SPEED_SERVO, Speed(100, rpm, 2000));
    g_q.emplace_back(tmp);
}

void analyze_F(std::string str)
{
    /* エラーチェック */
    if (str.size() <= 1)
    {
        std::stringstream sstr;
        sstr << __FUNCSIG__ << "\n\nsyntax err : " << str << std::endl;
        print_err(sstr.str().c_str());
    }

    /* 速度のデータを取得 */
    double object_speed = std::stod(str.substr(1));
    Speed speed = Speed(300, object_speed, 100);

    /* 速度の値を保存 */
    g_interpolation_speed = speed;
}

/*-----------------------------------------------
数値を表す文字かを判断する
-----------------------------------------------*/
bool is_numeric(char input_char)
{
    const std::vector<char> numerical_char{
        '+',
        '-',
        '.',
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
    };

    for (size_t i = 0; i < numerical_char.size(); i++)
    {
        if (input_char == numerical_char[i])
            return true;
    }
    return false;
}

/*-----------------------------------------------
*
* キューの中身を表示
*
-----------------------------------------------*/
void print_q(void)
{
    printf("\n-------------------------------------------------\n"
           "QUEUE\n\n");
    for (size_t i = 0; i < g_q.size(); i++)
    {
        switch (g_q[i].m_mode)
        {
        case MODE_CHANGE_SPEED_SPTEPPING_MOTOR:
            printf("change stepping motor speed\t%.2lf  %.2lf  %.2lf\n",
                   g_q[i].m_speed.m_object,
                   g_q[i].m_speed.m_ss,
                   g_q[i].m_speed.m_rate);
            break;
        case MODE_DRIVE_STEPPING_MOTOR:
            printf("drive stepping motor\t\t%.2lf\t%.2lf\n",
                   g_q[i].m_pos.m_x,
                   g_q[i].m_pos.m_z);
            break;
        case MODE_DRIVE_SERVO:
            printf("drive servo\n");
            break;
        case MODE_STOP_SERVO:
            printf("stop servo\n");
            break;
        case MODE_CHANGE_SPEED_SERVO:
            printf("change servo speed\t\t%.2lf  %.2lf  %.2lf\n",
                   g_q[i].m_speed.m_object,
                   g_q[i].m_speed.m_ss,
                   g_q[i].m_speed.m_rate);
            break;
        }
    }
    printf("-------------------------------------------------\n\n");
}

/*-----------------------------------------------
*
* 指定した index の直前の target_pos を返す
*
-----------------------------------------------*/
double find_previous_pos(size_t index, char X_or_Z)
{
    double pos;
    bool is_found = false;
    for (int j = index; j >= 1; j--)
    {
        if (g_q[j - 1].m_mode == MODE_DRIVE_STEPPING_MOTOR)
        {
            if (X_or_Z == 'X')
                pos = g_q[j - 1].m_pos.m_x;
            else if (X_or_Z == 'Z')
                pos = g_q[j - 1].m_pos.m_z;
            else
            {
                std::stringstream sstr;
                sstr << __FUNCSIG__ << "\n\ninvalid symbol : " << X_or_Z << std::endl;
                print_err(sstr.str().c_str());
            }

            is_found = true;
            break;
        }
    }

    /* 前の値が見つからなかった場合０を返す */
    if (is_found == false)
        pos = 0;

    return pos;
}