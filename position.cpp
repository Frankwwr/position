// 数据分组程序
// 一般而言

// 激活数据标定

// 读入数据（分组）
// 计算光产额平均化各个条的输出光子数（此方式先写一个接口，后续再补充方案）
// 事例分类

// 我建议将数据处理分为两个部分，通过解码将文件重新存储，将工作化为两个部分，将解码做好之后可以再在新的处理过程中做分析

// 将事例存储为两种情况(其中光字数允许部位整数由于需要加标定接口)
// 1、[光子数0][光子数1][条编号]                    using Single_Data_list= std::vector<Single_Data_struct>;
// 2、[光子数0][光子数1][光子数2][光子数3][条编号]   using Double_Data_list= std::vector<Double_Data_struct>;

// 处理数据
// 初步处理利用位置

// 输出位置分辨

#define WSL_SYS // 这个为以后想在win里用做个铺垫
#define SIZE 3

#include "include/RR_out.h"
#include "include/filemove.h"
#include "include/match.C"
#include "include/decode.h"

namespace fs = std::filesystem;

// 数据类型准备
struct Position_Temp
{
    int site[SIZE] = {0};
    double position[SIZE];
    double angle_k;
    double miu[SIZE] = {0};
};

// 存储初步解码的结果
using Photo_num = std::array<std::array<double, 2>, 8>; // 第二列数字存储TDCtime
using Photo_num_all = std::vector<Photo_num>;           // 光字数存储

struct Decode_result
{
    double values[16] = {0};
    int flag = 0; // 这里的flage指不包含信息，单条情况为-1，多条情况为1；
};
using Decode_result_all = std::vector<Decode_result>;

class RS_read : public rs
{
public:
    Decode_result_all decode_r;
    Photo_num_all photo_num_all;

    std::vector<int> entry_num; // 记录位置对于decode_r的entry

    Decode_result decode_single(Photo_num data);
    Decode_result_all averrage_result(Decode_result_all data_all); // 归一化处理
    RS_read(std::string file_name, std::string file_path, bool ant_flag = false);
};

/********************************************************

以下为代码正文

********************************************************/

std::vector<int> name_num = {0, 1, 2}; // 文件板子数

RS_read *Data_of_Board[SIZE];                                                     // 所有的数据
std::vector<std::array<std::pair<int, double>, 3>> match_data = match_get_data(); // 注意这里的数字需要改
// 这里的数据存储方案为先存entry后存对应的tdcTIME与板子顺序相同

// 获取现在有一个通道触发
int get_triger(Photo_num data)
{
    int num = 0;
    for (auto single_data : data)
        if (single_data[1] > 0)
            num++;
    return num;
}

Decode_result RS_read::decode_single(Photo_num data)
{
    Decode_result result;

    int map[4][4] = {{0, 13, 10, 6},
                     {4, 1, 7, 14},
                     {8, 5, 2, 11},
                     {12, 9, 15, 3}}; // 编码表

    // 给击中事例光子数排序
    std::array<std::pair<double, int>, 8> arrange_data;
    for (int i = 0; i < 8; i++)
    {
        arrange_data[i].first = data[i][0];
        arrange_data[i].second = i;
    }
    std::sort(arrange_data.begin(), arrange_data.end(), [](std::pair<double, int> &a, std::pair<double, int> &b)
              {
                  return a.first < b.first; // 按第二个数字升序排序, lambda 表达式
              });

    if (get_triger(data) <= 3)
    { // 单条击中情况
        int map_x, map_y;
        double photo_num_of_slice = 0;
        for (auto each : arrange_data)
        {
            bool flag_x = false, flag_y = false;
            if (each.second < 4 && flag_y == false)
            {
                photo_num_of_slice += each.first;
                map_y = 3 - each.second;
                flag_y = true;
            }
            else if (flag_x == false)
            {
                photo_num_of_slice += each.first;
                map_x = 7 - each.second;
                flag_x = true;
            }
            if (flag_x && flag_y)
                break;
        }
        auto a_code = map[map_x][map_y];
        result.values[a_code] = photo_num_of_slice;
        result.flag = -1;
        return result;
    }
    else // 多条击中的情况
    {
        int map_x[2], map_y[2];
        double photo_num_of_slice_x[2] = {0};
        double photo_num_of_slice_y[2] = {0};
        for (auto each : arrange_data)
        {
            bool flag_x[2] = {false, false}, flag_y[2] = {false, false};
            for (int i = 0; i < 2; i++)
                if (each.second < 4 && flag_y[i] == false)
                {
                    photo_num_of_slice_y[i] += each.first;
                    map_y[i] = 3 - each.second;
                    flag_y[i] = true;
                }
                else if (flag_x[i] == false)
                {
                    photo_num_of_slice_x[i] += each.first;
                    map_x[i] = 7 - each.second;
                    flag_x[i] = true;
                }
            if (flag_x[0] && flag_x[1] && flag_y[0] && flag_y[1])
                break;
        }
        auto a_code = decoode(map_x[0] * map_x[1], map_y[0] * map_y[1]);
        if (a_code[1] == 0)
            return result;
        int site_1, site_2;
        bool flag = false;
        for (auto num : a_code)
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (map[i][j] == num)
                    {
                        if (flag)
                        {
                            if (site_1 > i && site_2 > j)
                            {
                                result.values[a_code[1]] = photo_num_of_slice_y[0] + photo_num_of_slice_x[0];
                                result.values[a_code[0]] = photo_num_of_slice_y[1] + photo_num_of_slice_x[1];
                            }
                            else if (site_1 < i && site_2 < j)
                            {
                                result.values[a_code[0]] = photo_num_of_slice_y[0] + photo_num_of_slice_x[0];
                                result.values[a_code[1]] = photo_num_of_slice_y[1] + photo_num_of_slice_x[1];
                            }
                            else
                                return result;
                        }
                        else
                        {
                            site_1 = i;
                            site_2 = j;
                            flag = true;
                        }
                    }
        result.flag = 1;
        return result;
    }
}

RS_read::RS_read(std::string file_name, std::string file_path, bool ant_flag) : rs(7, file_name, file_path, ant_flag)
{
    // 初始化文件
    file = TFile::Open((file_path + file_name).c_str());
    TTree *tree;
    file->GetObject("board", tree);
    board b1(tree);

    DataContainer_all all_data = b1.Loop_all();
    for (auto each : all_data)
    {
        Photo_num get_one;
        for (int i = 0; i < 8; i++)
        {
            if (each[i][1] > 15000)
                get_one[i][0] = (each[i][1] * hgrate[i] + hgzero[i] - zero_point[i]) / hgpeak[i];
            else
                get_one[i][0] = (each[i][0] - zero_point[i]) / hgpeak[i];
            get_one[i][1] = each[i][2];
        }
        photo_num_all.push_back(get_one);
    }
    int array_num = 0;
    for (int i = 0; i < photo_num_all.size(); i++)
    {
        auto input = decode_single(photo_num_all.at(i));
        if (input.flag == 0)
            entry_num.push_back(array_num++);
        else
        {
            decode_r.push_back(input);
            entry_num.push_back(array_num++);
        }
    }
}

void init_data()
{
    for (int num : name_num)
    {
        std::cout << num << "基本rs类初始化完成" << std::endl;
#ifdef WSL_SYS
        // TString cwd = gSystem->WorkingDirectory();//用于得到当前位置
        //  给出需要读取的文件的名称
        std::string rootname = TString::Format("Board%d-Aligned.root", num).Data(); // 这个形成函数类似printf
        std::string filename = TString::Format("Board%d/", num).Data();
        moveFile(rootname, filename + rootname);
        // 初始化文件
        if (num == 0)
            new rs(7, rootname, filename, true);
        else
            new rs(7, rootname, filename);
#endif
    }
    for (int num : name_num)
    {
        std::string rootname = TString::Format("Board%d-Aligned.root", num).Data(); // 这个形成函数类似printf
        std::string filename = TString::Format("Board%d/", num).Data();
        if (num == 0)
            Data_of_Board[num] = new RS_read(rootname, filename, true);
        else
            Data_of_Board[num] = new RS_read(rootname, filename);
    }
}

std::pair<double, int> get_position_one_sub1(Decode_result data)
{
    for (int i = 0; i < 15; i++)
    {
        if (data.values[i] > 0)
            return {i * 11, i};
    }
    return {-12, -1};
}

std::pair<double, int> get_position_one_sub2(Decode_result data)
{
    for (int i = 0; i < 15; i++)
    {
        if (data.values[i] > 0)
        {
            double q1 = data.values[i], q2 = data.values[i + 1];
            return {double(i * q1 + (i + 11.0) * q2) / double(q1 + q2), i};
        }
    }
    return {-22, -2};
}

double func_miu(double q1, double q2)
{
    return (q2 - q1) / (q1 + q2);
}

double get_miu_of_2(Decode_result data)
{
    for (int i = 0; i < 15; i++)
    {
        if (data.values[i] > 0)
            return func_miu(data.values[i], data.values[i + 1]);
    }
    return 0;
}

std::vector<Position_Temp> get_position_one()
{
    std::vector<Position_Temp> result;
    for (auto each_match : match_data)
    {
        bool empty_flag = false;
        Position_Temp each_data;
        for (int i = 0; i < 3; i++)
        {
            Decode_result to_solve = Data_of_Board[i]->decode_r.at(Data_of_Board[i]->entry_num[each_match[i].first]);
            if (to_solve.flag == -1)
            {
                auto temp = get_position_one_sub1(to_solve);
                each_data.position[i] = temp.first;
                each_data.site[i] = temp.second;
            }
            else if (to_solve.flag == 1)
            {
                auto temp = get_position_one_sub2(to_solve);
                each_data.position[i] = temp.first;
                each_data.site[i] = temp.second;
                each_data.miu[i] = get_miu_of_2(to_solve);
            }
            else
                empty_flag = true;
        }
        if (empty_flag)
        {
            continue;
            empty_flag = false;
        }
        else
            result.push_back(each_data);
    }
    return result;
}

double angle_fix_func(double miu, double k, int flag)
{
    if (flag == 0)
        return miu * 5 * k;
    else
        return -miu * 5 * k;
}

std::vector<Position_Temp> angle_fix(std::vector<Position_Temp> data)
{
    for (auto &each_data : data)
    {
        each_data.angle_k = 51.0 / (each_data.position[2] - each_data.position[0]);
        for (int i = 0; i < 3; i++)
            if (each_data.miu[i] == 0)
            {
                if (each_data.site[i] % 2)
                    each_data.position[i] += 5 * each_data.angle_k;
                else
                    each_data.position[i] -= 5 * each_data.angle_k;
            }
            else
            {
                each_data.position[i] += angle_fix_func(each_data.miu[i], 1 / each_data.angle_k, each_data.site[i] % 2);
            }
    }
    return data;
}

void Draw_Graph(std::vector<Position_Temp> data){
    
}

void position()
{
    init_data();
    auto result = get_position_one();
    result = angle_fix(result); // 找角度可以复用
    Draw_Graph(result);
}

// 位置分辨函数
