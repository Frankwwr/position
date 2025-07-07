#define match_cxx
#include "match.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

std::vector<std::array<std::pair<int, double>, 3>> match::Loop()
{
   std::vector<std::array<std::pair<int, double>, 3>> result;

   if (fChain == 0)
      return result;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry = 0; jentry < nentries; jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0)
         break;
      nb = fChain->GetEntry(jentry);
      nbytes += nb;

      int count_of_num = 0;
      for (auto num : matchedBoard)
         count_of_num += num;
      if (count_of_num != 3)
         continue;

      // 下面为要输出的数据
      std::array<std::pair<int, double>, 3> each_data;
      for (int i = 0; i < 3; i++)
      {
         each_data[i].first = matchEntry[i];
         each_data[i].second = matchTime[i];
      }
      result.push_back(each_data);
   }
   return result;
}

std::vector<std::array<std::pair<int, double>, 3>> match_get_data()
{
   TTree *tree;
   TFile::Open("MatchEntries.root")->GetObject("match", tree);
   match match_raw_data(tree);
   return match_raw_data.Loop();
}
