#ifndef decode_h
#define decode_h

#include <list>
#include <array>
#include <vector>
#include <algorithm>

using Map_result = std::array<std::array<std::array<int, 2>, 6>, 6>;

Map_result decode_init()
{

    int map[4][4] = {{0, 13, 10, 6},
                     {4, 1, 7, 14},
                     {8, 5, 2, 11},
                     {12, 9, 15, 3}}; // 编码表

    // 用于定位的列表为两个数相乘
    std::list<int> lst1 = {2, 3, 4, 6, 8, 12};
    std::list<int> lst2 = {30, 35, 40, 42, 48, 56};

    Map_result result;

    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++)
            for (int k = 0; k < 4; k++)
                for (int l = k + 1; l < 4; l++)
                {
                    std::vector<int> nor_re;
                    nor_re.push_back(map[i][k]);
                    nor_re.push_back(map[j][l]);
                    nor_re.push_back(map[i][l]);
                    nor_re.push_back(map[j][k]);
                    std::sort(nor_re.begin(), nor_re.end());
                    int l1 = std::distance(lst1.begin(), std::find(lst1.begin(), lst1.end(), i * j)); // 给出两个变量的位置
                    int l2 = std::distance(lst2.begin(), std::find(lst2.begin(), lst2.end(), k * l));
                    result[l1][l2][0] = nor_re[0];
                    for (int num : nor_re)
                    {
                        if (num - result[l1][l2][0] == 1)
                            result[l1][l2][1] = num;
                        else
                            result[l1][l2][0] = num;
                    }
                }

    return result;
}

Map_result map_of_decode = decode_init(); // 初始化映射图

std::array<int, 2> decoode(int x, int y)
{
    // 用于定位的列表
    std::list<int> lst1 = {2, 3, 4, 6, 8, 12};
    std::list<int> lst2 = {30, 35, 40, 42, 48, 56};

    int l1 = std::distance(lst1.begin(), std::find(lst1.begin(), lst1.end(), x)); // 给出两个变量的位置
    int l2 = std::distance(lst2.begin(), std::find(lst2.begin(), lst2.end(), y));

    return map_of_decode[l1][l2];
}

#endif