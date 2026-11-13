//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 28 14:25:58 2021 by ROOT version 6.20/04
// from TTree T/Geant4 SBS Simulation
// found on file: g4sbsout_DigiEvents.root
//////////////////////////////////////////////////////////

#ifndef g4sbs_tree_acqu_light_h
#define g4sbs_tree_acqu_light_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

using namespace std;

class g4sbs_tree_acqu_light {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Double_t        ev_count;
   Double_t        ev_rate;
   Double_t        ev_solang;
   Double_t        ev_sigma;
   Double_t        ev_W2;
   Double_t        ev_xbj;
   Double_t        ev_Q2;
   Double_t        ev_th;
   Double_t        ev_ph;
   Double_t        ev_Aperp;
   Double_t        ev_Apar;
   Double_t        ev_Pt;
   Double_t        ev_Pl;
   Double_t        ev_vx;
   Double_t        ev_vy;
   Double_t        ev_vz;
   Double_t        ev_ep;
   Double_t        ev_np;
   Double_t        ev_epx;
   Double_t        ev_epy;
   Double_t        ev_epz;
   Double_t        ev_npx;
   Double_t        ev_npy;
   Double_t        ev_npz;
   Double_t        ev_nth;
   Double_t        ev_nph;
   Double_t        ev_pmperp;
   Double_t        ev_pmpar;
   Double_t        ev_pmparsm;
   Double_t        ev_z;
   Double_t        ev_phperp;
   Double_t        ev_phih;
   Double_t        ev_phiS;
   Double_t        ev_thetaS;
   Double_t        ev_MX2;
   Double_t        ev_Sx;
   Double_t        ev_Sy;
   Double_t        ev_Sz;
   Double_t        ev_s;
   Double_t        ev_t;
   Double_t        ev_u;
   Double_t        ev_costhetaCM;
   Double_t        ev_Egamma;
   Int_t           ev_nucl;
   Int_t           ev_fnucl;
   Int_t           ev_hadr;
   Int_t           ev_earmaccept;
   Int_t           ev_harmaccept;
   Double_t        ev_tdis_KE;
   Double_t        ev_tdis_sigmaELA;
   Double_t        ev_tdis_sigmaQE;
   Double_t        ev_tdis_sigmaSIDIS;
   Double_t        ev_tdis_sigmaDIS;
   Double_t        ev_tdis_sigmaTDIS;
   Double_t        ev_tdis_p1p;
   Double_t        ev_tdis_p2p;
   Double_t        ev_tdis_pip;
   Double_t        ev_tdis_p1px;
   Double_t        ev_tdis_p1py;
   Double_t        ev_tdis_p1pz;
   Double_t        ev_tdis_p2px;
   Double_t        ev_tdis_p2py;
   Double_t        ev_tdis_p2pz;
   Double_t        ev_tdis_pipx;
   Double_t        ev_tdis_pipy;
   Double_t        ev_tdis_pipz;
   Double_t        ev_tdis_p1th;
   Double_t        ev_tdis_p1ph;
   Double_t        ev_tdis_p2th;
   Double_t        ev_tdis_p2ph;
   Double_t        ev_tdis_pith;
   Double_t        ev_tdis_piph;
   Double_t        ev_tdis_ya;
   Double_t        ev_tdis_y;
   Double_t        ev_tdis_f2p;
   Double_t        ev_tdis_f2pi;
   Double_t        ev_tdis_ypi;
   Int_t           SBS_mTPC_hit_nhits;
   // vector<int>     *SBS_mTPC_hit_cell;
   // vector<double>  *SBS_mTPC_hit_xcell;
   // vector<double>  *SBS_mTPC_hit_ycell;
   // vector<double>  *SBS_mTPC_hit_zcell;
   // vector<double>  *SBS_mTPC_hit_xcellg;
   // vector<double>  *SBS_mTPC_hit_ycellg;
   // vector<double>  *SBS_mTPC_hit_zcellg;
   // vector<double>  *SBS_mTPC_hit_xhit;
   // vector<double>  *SBS_mTPC_hit_yhit;
   // vector<double>  *SBS_mTPC_hit_zhit;
   vector<double>  *SBS_mTPC_hit_xhitg;
   vector<double>  *SBS_mTPC_hit_yhitg;
   vector<double>  *SBS_mTPC_hit_zhitg;
   vector<double>  *SBS_mTPC_hit_edep;
   vector<double>  *SBS_mTPC_hit_tavg;
   // vector<double>  *SBS_mTPC_hit_trms;
   // vector<double>  *SBS_mTPC_hit_tmin;
   // vector<double>  *SBS_mTPC_hit_tmax;
   // vector<double>  *SBS_mTPC_hit_hitL;
   // vector<double>  *SBS_mTPC_hit_px;
   // vector<double>  *SBS_mTPC_hit_py;
   // vector<double>  *SBS_mTPC_hit_pz;
   // vector<double>  *SBS_mTPC_hit_px_v;
   // vector<double>  *SBS_mTPC_hit_py_v;
   // vector<double>  *SBS_mTPC_hit_pz_v;
   vector<int>     *SBS_mTPC_hit_trid;
   vector<int>     *SBS_mTPC_hit_mid;
   vector<int>     *SBS_mTPC_hit_pid;
   // Double_t        AcquMCPrimaries_Vx;
   // Double_t        AcquMCPrimaries_Vy;
   // Double_t        AcquMCPrimaries_Vz;
   // Double_t        AcquMCPrimaries_Px;
   // Double_t        AcquMCPrimaries_Py;
   // Double_t        AcquMCPrimaries_Pz;
   // Double_t        AcquMCPrimaries_Pt;
   // Double_t        AcquMCPrimaries_E;
   // Int_t           AcquMCPrimaries_pid;

   // List of branches
   TBranch        *b_ev;   //!
   TBranch        *b_ev_tdis;   //!
   // TBranch        *b_SBS_mTPC_det_esum;   //!
   TBranch        *b_SBS_mTPC_hit_nhits;   //!
   // TBranch        *b_SBS_mTPC_hit_cell;   //!
   // TBranch        *b_SBS_mTPC_hit_xcell;   //!
   // TBranch        *b_SBS_mTPC_hit_ycell;   //!
   // TBranch        *b_SBS_mTPC_hit_zcell;   //!
   // TBranch        *b_SBS_mTPC_hit_xcellg;   //!
   // TBranch        *b_SBS_mTPC_hit_ycellg;   //!
   // TBranch        *b_SBS_mTPC_hit_zcellg;   //!
   // TBranch        *b_SBS_mTPC_hit_xhit;   //!
   // TBranch        *b_SBS_mTPC_hit_yhit;   //!
   // TBranch        *b_SBS_mTPC_hit_zhit;   //!
   TBranch        *b_SBS_mTPC_hit_xhitg;   //!
   TBranch        *b_SBS_mTPC_hit_yhitg;   //!
   TBranch        *b_SBS_mTPC_hit_zhitg;   //!
   TBranch        *b_SBS_mTPC_hit_edep;   //!
   TBranch        *b_SBS_mTPC_hit_tavg;   //!
   // TBranch        *b_SBS_mTPC_hit_trms;   //!
   // TBranch        *b_SBS_mTPC_hit_tmin;   //!
   // TBranch        *b_SBS_mTPC_hit_tmax;   //!
   // TBranch        *b_SBS_mTPC_hit_hitL;   //!
   // TBranch        *b_SBS_mTPC_hit_px;   //!
   // TBranch        *b_SBS_mTPC_hit_py;   //!
   // TBranch        *b_SBS_mTPC_hit_pz;   //!
   // TBranch        *b_SBS_mTPC_hit_px_v;   //!
   // TBranch        *b_SBS_mTPC_hit_py_v;   //!
   // TBranch        *b_SBS_mTPC_hit_pz_v;   //!
   TBranch        *b_SBS_mTPC_hit_trid;   //!
   TBranch        *b_SBS_mTPC_hit_mid;   //!
   TBranch        *b_SBS_mTPC_hit_pid;   //!
   // TBranch        *b_AcquMCPrimaries_Vx;   //!
   // TBranch        *b_AcquMCPrimaries_Vy;   //!
   // TBranch        *b_AcquMCPrimaries_Vz;   //!
   // TBranch        *b_AcquMCPrimaries_Px;   //!
   // TBranch        *b_AcquMCPrimaries_Py;   //!
   // TBranch        *b_AcquMCPrimaries_Pz;   //!
   // TBranch        *b_AcquMCPrimaries_Pt;   //!
   // TBranch        *b_AcquMCPrimaries_E;   //!
   // TBranch        *b_AcquMCPrimaries_pid;   //!

   g4sbs_tree_acqu_light(TTree *tree=0);
   virtual ~g4sbs_tree_acqu_light();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef g4sbs_tree_acqu_light_cxx
g4sbs_tree_acqu_light::g4sbs_tree_acqu_light(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("g4sbsout_DigiEvents.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("g4sbsout_DigiEvents.root");
      }
      f->GetObject("T",tree);

   }
   Init(tree);
}

g4sbs_tree_acqu_light::~g4sbs_tree_acqu_light()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t g4sbs_tree_acqu_light::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t g4sbs_tree_acqu_light::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void g4sbs_tree_acqu_light::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   // SBS_mTPC_hit_cell = 0;
   // SBS_mTPC_hit_xcell = 0;
   // SBS_mTPC_hit_ycell = 0;
   // SBS_mTPC_hit_zcell = 0;
   // SBS_mTPC_hit_xcellg = 0;
   // SBS_mTPC_hit_ycellg = 0;
   // SBS_mTPC_hit_zcellg = 0;
   // SBS_mTPC_hit_xhit = 0;
   // SBS_mTPC_hit_yhit = 0;
   // SBS_mTPC_hit_zhit = 0;
   SBS_mTPC_hit_xhitg = 0;
   SBS_mTPC_hit_yhitg = 0;
   SBS_mTPC_hit_zhitg = 0;
   SBS_mTPC_hit_edep = 0;
   SBS_mTPC_hit_tavg = 0;
   // SBS_mTPC_hit_trms = 0;
   // SBS_mTPC_hit_tmin = 0;
   // SBS_mTPC_hit_tmax = 0;
   // SBS_mTPC_hit_hitL = 0;
   // SBS_mTPC_hit_px = 0;
   // SBS_mTPC_hit_py = 0;
   // SBS_mTPC_hit_pz = 0;
   // SBS_mTPC_hit_px_v = 0;
   // SBS_mTPC_hit_py_v = 0;
   // SBS_mTPC_hit_pz_v = 0;
   SBS_mTPC_hit_trid = 0;
   SBS_mTPC_hit_mid = 0;
   SBS_mTPC_hit_pid = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("ev", 1);
   fChain->SetBranchStatus("ev_tdis", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.nhits", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.xhitg", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.yhitg", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.zhitg", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.zhitg", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.edep", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.tavg", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.trid", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.mid", 1);
   fChain->SetBranchStatus("SBS.mTPC.hit.pid", 1);
   // fChain->SetBranchStatus("AcquMCPrimaries.Vz", 1);
   // fChain->SetBranchStatus("AcquMCPrimaries.Px", 1);
   // fChain->SetBranchStatus("AcquMCPrimaries.Py", 1);
   // fChain->SetBranchStatus("AcquMCPrimaries.Pz", 1);
   // fChain->SetBranchStatus("AcquMCPrimaries.Pt", 1);

   fChain->SetBranchAddress("ev", &ev_count, &b_ev);
   fChain->SetBranchAddress("ev_tdis", &ev_tdis_KE, &b_ev_tdis);
   // fChain->SetBranchAddress("SBS.mTPC.det.esum", &SBS_mTPC_det_esum, &b_SBS_mTPC_det_esum);
   fChain->SetBranchAddress("SBS.mTPC.hit.nhits", &SBS_mTPC_hit_nhits, &b_SBS_mTPC_hit_nhits);
   // fChain->SetBranchAddress("SBS.mTPC.hit.cell", &SBS_mTPC_hit_cell, &b_SBS_mTPC_hit_cell);
   // fChain->SetBranchAddress("SBS.mTPC.hit.xcell", &SBS_mTPC_hit_xcell, &b_SBS_mTPC_hit_xcell);
   // fChain->SetBranchAddress("SBS.mTPC.hit.ycell", &SBS_mTPC_hit_ycell, &b_SBS_mTPC_hit_ycell);
   // fChain->SetBranchAddress("SBS.mTPC.hit.zcell", &SBS_mTPC_hit_zcell, &b_SBS_mTPC_hit_zcell);
   // fChain->SetBranchAddress("SBS.mTPC.hit.xcellg", &SBS_mTPC_hit_xcellg, &b_SBS_mTPC_hit_xcellg);
   // fChain->SetBranchAddress("SBS.mTPC.hit.ycellg", &SBS_mTPC_hit_ycellg, &b_SBS_mTPC_hit_ycellg);
   // fChain->SetBranchAddress("SBS.mTPC.hit.zcellg", &SBS_mTPC_hit_zcellg, &b_SBS_mTPC_hit_zcellg);
   // fChain->SetBranchAddress("SBS.mTPC.hit.xhit", &SBS_mTPC_hit_xhit, &b_SBS_mTPC_hit_xhit);
   // fChain->SetBranchAddress("SBS.mTPC.hit.yhit", &SBS_mTPC_hit_yhit, &b_SBS_mTPC_hit_yhit);
   // fChain->SetBranchAddress("SBS.mTPC.hit.zhit", &SBS_mTPC_hit_zhit, &b_SBS_mTPC_hit_zhit);
   fChain->SetBranchAddress("SBS.mTPC.hit.xhitg", &SBS_mTPC_hit_xhitg, &b_SBS_mTPC_hit_xhitg);
   fChain->SetBranchAddress("SBS.mTPC.hit.yhitg", &SBS_mTPC_hit_yhitg, &b_SBS_mTPC_hit_yhitg);
   fChain->SetBranchAddress("SBS.mTPC.hit.zhitg", &SBS_mTPC_hit_zhitg, &b_SBS_mTPC_hit_zhitg);
   fChain->SetBranchAddress("SBS.mTPC.hit.edep", &SBS_mTPC_hit_edep, &b_SBS_mTPC_hit_edep);
   fChain->SetBranchAddress("SBS.mTPC.hit.tavg", &SBS_mTPC_hit_tavg, &b_SBS_mTPC_hit_tavg);
   // fChain->SetBranchAddress("SBS.mTPC.hit.trms", &SBS_mTPC_hit_trms, &b_SBS_mTPC_hit_trms);
   // fChain->SetBranchAddress("SBS.mTPC.hit.tmin", &SBS_mTPC_hit_tmin, &b_SBS_mTPC_hit_tmin);
   // fChain->SetBranchAddress("SBS.mTPC.hit.tmax", &SBS_mTPC_hit_tmax, &b_SBS_mTPC_hit_tmax);
   // fChain->SetBranchAddress("SBS.mTPC.hit.hitL", &SBS_mTPC_hit_hitL, &b_SBS_mTPC_hit_hitL);
   // fChain->SetBranchAddress("SBS.mTPC.hit.px", &SBS_mTPC_hit_px, &b_SBS_mTPC_hit_px);
   // fChain->SetBranchAddress("SBS.mTPC.hit.py", &SBS_mTPC_hit_py, &b_SBS_mTPC_hit_py);
   // fChain->SetBranchAddress("SBS.mTPC.hit.pz", &SBS_mTPC_hit_pz, &b_SBS_mTPC_hit_pz);
   // fChain->SetBranchAddress("SBS.mTPC.hit.px_v", &SBS_mTPC_hit_px_v, &b_SBS_mTPC_hit_px_v);
   // fChain->SetBranchAddress("SBS.mTPC.hit.py_v", &SBS_mTPC_hit_py_v, &b_SBS_mTPC_hit_py_v);
   // fChain->SetBranchAddress("SBS.mTPC.hit.pz_v", &SBS_mTPC_hit_pz_v, &b_SBS_mTPC_hit_pz_v);
   fChain->SetBranchAddress("SBS.mTPC.hit.trid", &SBS_mTPC_hit_trid, &b_SBS_mTPC_hit_trid);
   fChain->SetBranchAddress("SBS.mTPC.hit.mid", &SBS_mTPC_hit_mid, &b_SBS_mTPC_hit_mid);
   fChain->SetBranchAddress("SBS.mTPC.hit.pid", &SBS_mTPC_hit_pid, &b_SBS_mTPC_hit_pid);
   // fChain->SetBranchAddress("AcquMCPrimaries.Vx", &AcquMCPrimaries_Vx, &b_AcquMCPrimaries_Vx);
   // fChain->SetBranchAddress("AcquMCPrimaries.Vy", &AcquMCPrimaries_Vy, &b_AcquMCPrimaries_Vy);
   // fChain->SetBranchAddress("AcquMCPrimaries.Vz", &AcquMCPrimaries_Vz, &b_AcquMCPrimaries_Vz);
   // fChain->SetBranchAddress("AcquMCPrimaries.Px", &AcquMCPrimaries_Px, &b_AcquMCPrimaries_Px);
   // fChain->SetBranchAddress("AcquMCPrimaries.Py", &AcquMCPrimaries_Py, &b_AcquMCPrimaries_Py);
   // fChain->SetBranchAddress("AcquMCPrimaries.Pz", &AcquMCPrimaries_Pz, &b_AcquMCPrimaries_Pz);
   // fChain->SetBranchAddress("AcquMCPrimaries.Pt", &AcquMCPrimaries_Pt, &b_AcquMCPrimaries_Pt);
   // fChain->SetBranchAddress("AcquMCPrimaries.E", &AcquMCPrimaries_E, &b_AcquMCPrimaries_E);
   // fChain->SetBranchAddress("AcquMCPrimaries.pid", &AcquMCPrimaries_pid, &b_AcquMCPrimaries_pid);
   Notify();
}

Bool_t g4sbs_tree_acqu_light::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void g4sbs_tree_acqu_light::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t g4sbs_tree_acqu_light::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef g4sbs_tree_acqu_light_cxx
