#define board_cxx
#include "boardRR.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <iostream>
#include <sstream>
#include <TTree.h>
#include <TF1.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <fstream>
#include <algorithm> // 包含排序算法
#include <filesystem>
#include <TLatex.h>
#include <vector>
#include <cmath>

// 可以在宏定义的时候提供一个有关的值使读取方式可以以8位8位的读出

DataContainer board::Loop_single(int mark) // 这个函数会给出两个增益的函数
{
    DataContainer result;
    if (fChain == 0)
        return result;
    Long64_t nentries = fChain->GetEntriesFast();
    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        DataBlock each_data;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0)
            break;
        fChain->GetEntry(jentry);
        if (ientry < 0)
            break;
        // 下面这段使用于逻辑2的结合
        // int i_with = (mark % 2 ? mark - 1 : mark + 1);
        each_data[0] = HGamp[mark];
        each_data[1] = LGamp[mark];
        each_data[2] = TDCTime[mark];
        result.push_back(each_data);
    }
    return result;
}

DataContainer_all board::Loop_all() // 给出实际上所有的数据存储在内存中。
{
    DataContainer_all result;
    if (fChain == 0)
        return result;
    Long64_t nentries = fChain->GetEntriesFast();
    for (Long64_t jentry = 0; jentry < (nentries < 4800000 ? nentries : 4800000); jentry++)
    {
        DataBlock_all each_data;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0)
            break;
        fChain->GetEntry(jentry);
        if (ientry < 0)
            break;
        for (int i = 8; i < 16; i++)
        {
            // 下面这段使用于逻辑2的结合
            // int i_with = (i % 2 ? i - 1 : i + 1);
            each_data[i - 8][0] = HGamp[i];
            each_data[i - 8][1] = LGamp[i];
            each_data[i - 8][2] = TDCTime[i];
        }
        result.push_back(each_data);
    }
    return result;
}

int exchange(int in) // 用于修正反置的第十块板
{
    return 31 - in;
}

DataContainer board::Loop_single_anti(int anti_mark) // 这个函数会给出两个增益的函数
{
    // 重定义mark
    int mark = exchange(anti_mark);

    DataContainer result;
    if (fChain == 0)
        return result;
    Long64_t nentries = fChain->GetEntriesFast();
    for (Long64_t jentry = 0; jentry < nentries; jentry++)
    {
        DataBlock each_data;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0)
            break;
        fChain->GetEntry(jentry);
        if (ientry < 0)
            break;
        // int i_with = (mark % 2 ? mark - 1 : mark + 1);
        each_data[0] = HGamp[exchange(mark)];
        each_data[1] = LGamp[exchange(mark)];
        each_data[2] = TDCTime[exchange(mark)];
        result.push_back(each_data);
    }
    return result;
}

DataContainer_all board::Loop_all_anti()
{ // 用于读取某块反置的板子
    DataContainer_all result;
    if (fChain == 0)
        return result;
    Long64_t nentries = fChain->GetEntriesFast();
    for (Long64_t jentry = 0; jentry < (nentries < 4800000 ? nentries : 4800000); jentry++)
    {
        DataBlock_all each_data;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0)
            break;
        fChain->GetEntry(jentry);
        if (ientry < 0)
            break;
        for (int i = 8; i < 16; i++)
        {
            // 下面这段使用于逻辑2的结合
            // int i_with = (i % 2 ? i - 1 : i + 1);
            each_data[i - 8][0] = HGamp[exchange(i)];
            each_data[i - 8][1] = LGamp[exchange(i)];
            each_data[i - 8][2] = TDCTime[exchange(i)];
        }
        result.push_back(each_data);
    }
    return result;
}