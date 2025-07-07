//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Jul  5 17:06:54 2025 by ROOT version 6.34.06
// from TTree match/matched entries
// found on file: ./MatchEntries.root
//////////////////////////////////////////////////////////

#ifndef match_h
#define match_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class match
{
public:
   TTree *fChain;  //! pointer to the analyzed TTree or TChain
   Int_t fCurrent; //! current Tree number in a TChain

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t counter;
   Int_t matchedBoard[3]; //[counter]
   Int_t matchFlag[3];
   ULong64_t matchEntry[3];
   Double_t matchTime[3];
   Double_t lastSeg[3];
   Double_t nextSeg[3];
   Double_t interval[3];

   // List of branches
   TBranch *b_counter;      //!
   TBranch *b_matchedBoard; //!
   TBranch *b_matchFlag;    //!
   TBranch *b_matchEntry;   //!
   TBranch *b_matchTime;    //!
   TBranch *b_lastSeg;      //!
   TBranch *b_nextSeg;      //!
   TBranch *b_interval;     //!

   match(TTree *tree = 0);
   virtual ~match();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);
   virtual std::vector<std::array<std::pair<int, double>,3>> Loop();
   virtual bool Notify();
   virtual void Show(Long64_t entry = -1);
};

#endif

#ifdef match_cxx
match::match(TTree *tree) : fChain(0)
{
   // if parameter tree is not specified (or zero), connect the file
   // used to generate this class and read the Tree.
   if (tree == 0)
   {
      TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject("./MatchEntries.root");
      if (!f || !f->IsOpen())
      {
         f = new TFile("./MatchEntries.root");
      }
      f->GetObject("match", tree);
   }
   Init(tree);
}

match::~match()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Int_t match::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}
Long64_t match::LoadTree(Long64_t entry)
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

void match::Init(TTree *tree)
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

   fChain->SetBranchAddress("counter", &counter, &b_counter);
   fChain->SetBranchAddress("matchedBoard", matchedBoard, &b_matchedBoard);
   fChain->SetBranchAddress("matchFlag", matchFlag, &b_matchFlag);
   fChain->SetBranchAddress("matchEntry", matchEntry, &b_matchEntry);
   fChain->SetBranchAddress("matchTime", matchTime, &b_matchTime);
   fChain->SetBranchAddress("lastSeg", lastSeg, &b_lastSeg);
   fChain->SetBranchAddress("nextSeg", nextSeg, &b_nextSeg);
   fChain->SetBranchAddress("interval", interval, &b_interval);
   Notify();
}

bool match::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void match::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t match::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
#endif // #ifdef match_cxx
