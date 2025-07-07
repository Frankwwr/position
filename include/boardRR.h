//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct 21 16:25:29 2024 by ROOT version 6.30/06
// from TTree board/board
// found on file: ./board10.21.root
//////////////////////////////////////////////////////////

#ifndef board_h
#define board_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

using DataBlock = std::array<double, 3>; // 数据结构为lg，hg，tdctime
using DataContainer = std::vector<DataBlock>;
using DataBlock_all = std::array<std::array<double, 3>, 8>; // 8为不同通道道数；
using DataContainer_all = std::vector<DataBlock_all>;

class board
{
public:
   TTree *fChain;  //! pointer to the analyzed TTree or TChain
   Int_t fCurrent; //! current Tree number in a TChain

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t id;
   UChar_t FiredCount;
   UChar_t FiredCh[32]; //[FiredCount]
   Double_t HGamp[32];
   Double_t LGamp[32];
   Double_t TDCTime[33];

   // List of branches
   TBranch *b_id;         //!
   TBranch *b_FiredCount; //!
   TBranch *b_FiredCh;    //!
   TBranch *b_HGamp;      //!
   TBranch *b_LGamp;      //!
   TBranch *b_TDCTime;    //!

   board(TTree *tree = 0);
   virtual ~board();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);

   virtual DataContainer Loop_single(int mark);
   virtual DataContainer_all Loop_all();
   virtual DataContainer_all Loop_all_anti();
   virtual DataContainer Loop_single_anti(int mark);

   virtual Bool_t Notify();
   virtual void Show(Long64_t entry = -1);
   virtual Long64_t GetEntries();
};

#endif

#ifdef board_cxx
board::board(TTree *tree) : fChain(0)
{
   // if parameter tree is not specified (or zero), connect the file
   // used to generate this class and read the Tree.
   if (tree == 0)
   {
      TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject("./board10.21.root");
      if (!f || !f->IsOpen())
      {
         f = new TFile("./board10.21.root");
      }
      f->GetObject("board", tree);
   }
   Init(tree);
}

board::~board()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Long64_t board::GetEntries()
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntriesFast();
}

Int_t board::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}

Long64_t board::LoadTree(Long64_t entry)
{
   // Set the environment to read one entry
   if (!fChain)
      return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0)
      return centry;
   if (fChain->GetTreeNumber() != fCurrent)
   {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void board::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree)
      return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("id", &id, &b_id);
   fChain->SetBranchAddress("FiredCount", &FiredCount, &b_FiredCount);
   fChain->SetBranchAddress("FiredCh", FiredCh, &b_FiredCh);
   fChain->SetBranchAddress("HGamp", HGamp, &b_HGamp);
   fChain->SetBranchAddress("LGamp", LGamp, &b_LGamp);
   fChain->SetBranchAddress("TDCTime", TDCTime, &b_TDCTime);
   Notify();
}

Bool_t board::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void board::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t board::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
#endif // #ifdef board_cxx
